#include "rpi.h"

typedef struct {
    int steps;
    unsigned DIR;
    unsigned STEP;
    // used for interrupts:
    int speed; 
    unsigned in_job; 
} stepper_t;

/**
 * Initialize a stepper w/o interrupts
 */
stepper_t * stepper_init(unsigned DIR, unsigned STEP){
    unimplemented();
}

/**
 * Steps the motor forward, and updates stepper's steps
 */ 
void stepper_step_forward(stepper_t * stepper){
    unimplemented();
}

/**
 * Steps the motor backward, and updates stepper's steps
 */ 
void stepper_step_backward(stepper_t * stepper){
    unimplemented();
}

/**
 * returns current position the stepper is at in steps,
 */
int stepper_get_position(stepper_t * stepper){
    unimplemented();
}

#define DIR 21
#define STEP 20

void test_stepper_without_interrupts(){
    stepper_t * my_stepper = stepper_init(DIR, STEP);
    unsigned num_steps = 50;
    for(unsigned i = 0; i < num_steps; i++){
        stepper_step_forward(my_stepper);
        delay_us(100000); // slow. how fast can you make this?
    }
    assert(stepper_get_position(my_stepper) == num_steps);
}

static unsigned stepper_t * int_enabled_stepper; 
static unsigned int_time_quanta_size_usec = 100;
static unsigned int_time_quanta_count = 100;


// computes x % y. Source: https://stackoverflow.com/questions/12486883/write-a-modulo-function-using-only-addition-subtraction
static unsigned mod_bld(unsigned x, unsigned y){
    while (y <= x)
        y <<= 1;
    while (x >= y) {
        while (y > x)
            y >>= 1;
        x -= y;
    }
    return x;
}

/**
 * call stepper_step_XXX(int_enabled_stepper) at the right time_quanta_count 
 * here, based on int_enabled_stepper->speed. Speed of 0 should be no movement,
 * and negative speed should turn in opposite direction as positive speed.
 */
void stepper_int_handler(){
    int_time_quanta_count++;
    unimplemented();
}

void interrupt_vector(unsigned pc){
    stepper_int_handler();
}

/**
 * Feel free to change.
 */ 
void init_stepper_int(stepper_t * stepper){
    int_enabled_stepper = stepper;
    int_enabled_stepper->speed = 0; 
    int_init();
    cycle_cnt_init();
    timer_interrupt_init(int_time_quanta_size_usec/10); // divide by 10 b/c currently, my timer interrupt base unit is 10 usec.
    system_enable_interrupts();
}

void set_stepper_speed(stepper_t * stepper, int speed){
    unimplemented();
}

void test_stepper_with_interrupts(){
    stepper_t * my_stepper = stepper_init(DIR, STEP);
    init_stepper_int(my_stepper);
    set_stepper_speed(my_stepper, 10);
    delay_sec(1);
    set_stepper_speed(my_stepper, -10);
    delay_sec(1);
    set_stepper_speed(my_stepper, 0);
    assert(stepper_get_position(my_stepper) == 0); // this may not be the most robust test
}

int notmain(){
    printk("Beginning stepper tests\n");
    test_stepper_without_interrupts();
    // test_stepper_with_interrupts();
    printk("Done with stepper tests\n");
    clean_reboot();
}