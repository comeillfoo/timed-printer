#include <linux/module.h>      /* essential for modules' macros */
#include <linux/moduleparam.h> /* essential for parameters */
#include <linux/stat.h>        /* essential for parameters permissions */
#include <linux/kernel.h>      /* essential for KERN_INFO */
#include <linux/jiffies.h>     /* essential for jiffies */
#include <linux/timer.h>       /* essential for timers */


MODULE_LICENSE( "Dual MIT/GPL" );
MODULE_AUTHOR( "come_ill_foo" );
MODULE_DESCRIPTION( "Periodically writes 'Hello from kernel module' to the common file" );


#define DEFAULT_FILE_PATH "/home/hello"


static unsigned int usecs = 5000;
module_param( usecs, uint, S_IRUGO | S_IWUSR );

static char* file_path = DEFAULT_FILE_PATH;
module_param( file_path, charp, S_IRUGO | S_IWUSR );


static struct timer_list main_timer;


static void timed_print( struct timer_list* timer ) {
  printk( KERN_INFO "Hello from kernel module\n" );
}


static int __init init_tprinter( void ) {
  int is_active = 0;
  printk( KERN_INFO "tprinter inited\n" );

  timer_setup( &main_timer, timed_print, 0 );
  is_active = mod_timer( &main_timer, jiffies + msecs_to_jiffies(5000) );
  return 0;
}


static void __exit cleanup_tprinter( void ) {
  printk( KERN_INFO "tprinter cleaned up\n" );

  del_timer( &main_timer );
}

module_init( init_tprinter );
module_exit( cleanup_tprinter );
