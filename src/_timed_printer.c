#include <linux/module.h>
#include <linux/moduleparam.h>

#include <linux/stat.h>
#include <linux/interrupt.h>
#include <linux/hrtimer.h>
#include <linux/sched.h>

#include <linux/fs.h>
#include <linux/err.h>

#include <linux/string.h>

#include <linux/workqueue.h>


MODULE_LICENSE( "Dual MIT/GPL" );
MODULE_AUTHOR( "come_ill_foo" );
MODULE_DESCRIPTION( "Periodically writes 'Hello from kernel module' to the common file" );


#define MOD_NAME "tprinter"

#define DEFAULT_TIMEOUT   5
#define DEFAULT_FILE_PATH "/tmp/hello_tprinter"


static long secs = DEFAULT_TIMEOUT;
module_param( secs, long, S_IRUSR | S_IWUSR );

static char* common_fpath = DEFAULT_FILE_PATH;
module_param( common_fpath, charp, S_IRUSR | S_IWUSR );

static loff_t f_offset = 0;

static struct hrtimer main_timer;
static ktime_t period;

static int append_err = 0;
static int append( const char* fpath );

static void workqueue_cb( struct work_struct* work );


DECLARE_WORK( workqueue, workqueue_cb );

static void workqueue_cb( struct work_struct* work ) {
  append_err = append( common_fpath );
}
 

static enum hrtimer_restart timed_print( struct hrtimer* timer ) {
  if ( append_err )
    return HRTIMER_NORESTART;

  schedule_work( &workqueue );
  hrtimer_forward_now( timer, period );

  return HRTIMER_RESTART;
}


static int __init init_tprinter( void ) {
  printk( KERN_INFO MOD_NAME": module inited\n" );

  period = ktime_set( secs, 0 ); // seconds, nanoseconds
  hrtimer_init( &main_timer, CLOCK_REALTIME, HRTIMER_MODE_REL );
  main_timer.function = timed_print;
  hrtimer_start( &main_timer, period, HRTIMER_MODE_REL );

  return 0;
}


static void __exit cleanup_tprinter( void ) {
  printk( KERN_INFO MOD_NAME ": module cleaned up\n" );
  
  hrtimer_cancel( &main_timer );
}

module_init( init_tprinter );
module_exit( cleanup_tprinter );


static const char* text = "Hello from kernel module\n";
static const size_t length = 25;


static int append( const char* fpath ) {
  ssize_t count = 0;
  int err = 0;
  struct file* file_p = NULL;

  file_p = filp_open( common_fpath, O_RDWR | O_CREAT | O_APPEND, 0666 );
  if ( IS_ERR( file_p ) ) {
    printk( KERN_ALERT MOD_NAME ": can't open the file %s with error %ld\n", common_fpath, PTR_ERR( file_p ) );
    return PTR_ERR( file_p );
  }

  count = kernel_write( file_p, text, length, &f_offset );

  printk( KERN_INFO MOD_NAME ": successfully appended [ %zu ] symbols on line [ %lld ] \n", count, f_offset );

  err = filp_close( file_p, NULL );
  if ( err ) {
    printk( KERN_ALERT MOD_NAME ": can't close the file %s properly\n", common_fpath );
    return err;
  }

  return 0;
}