#ifndef __NORMAL_MODE_H
#define __NORMAL_MODE_H

/**@brief Function for initializing power management.
 */
void power_management_init(void);

/**@brief Function for handling the idle state (main loop).
 *
 * @details If there is no pending log operation, then sleep until next the next event occurs.
 */
void idle_state_handle(void);


/**@brief Function for application normal_mode main entry.
 */
void normal_mode(void);


#endif

