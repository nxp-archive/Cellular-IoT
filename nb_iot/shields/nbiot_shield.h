#ifndef __NBIOT_SHIELD_H__
#define __NBIOT_SHIELD_H__

#include "FreeRTOS.h"

/* Select specific shield support */
#ifndef NBIOTSHIELD_IS
#define NBIOTSHIELD_IS NBIOTSHIELD_IS_SKYWIRE
#endif

/* Include shields support */
#if defined(NBIOTSHIELD_IS)
#   if (NBIOTSHIELD_IS) == (NBIOTSHIELD_IS_SKYWIRE)
#       include "nbiot_shield_skywire.h"
#   else
#       error "Unsupported shield selected !"
#   endif
#else
#   error "No shield is selected !"
#endif

#include "nbiot_env_port.h"

/* define IRQ priority level */
#ifndef NBIOTSHIELD_UART_IRQ_PRIORITY
#   define NBIOTSHIELD_UART_IRQ_PRIORITY (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1)
#endif
#ifndef NBIOTSHIELD_DMA_IRQ_PRIORITY
#   define NBIOTSHIELD_DMA_IRQ_PRIORITY (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1)
#endif
#ifndef NBIOTSHIELD_WLAN_IRQ_PRIORITY
#   define NBIOTSHIELD_WLAN_IRQ_PRIORITY (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1)
#endif

/* Fn prototypes, which need to be implemented */
int NBIOTSHIELD_Init( uint32_t baudrate );
int NBIOTSHIELD_USARTConfig( uint32_t baudrate );
int NBIOTSHIELD_DMAConfig( void );
int NBIOTSHIELD_TimerConfig( void );

#endif
