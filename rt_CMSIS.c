/*----------------------------------------------------------------------------
 *      CMSIS-RTOS  -  RTX
 *----------------------------------------------------------------------------
 *      Name:    rt_CMSIS.c
 *      Purpose: CMSIS RTOS API
 *      Rev.:    V4.82
 *----------------------------------------------------------------------------
 *
 * Copyright (c) 1999-2009 KEIL, 2009-2017 ARM Germany GmbH. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *---------------------------------------------------------------------------*/
#define __CORTEX_M0
#define __CMSIS_GENERIC

#if defined (__CORTEX_M4) || defined (__CORTEX_M4F)
  #include "core_cm4.h"
#elif defined (__CORTEX_M3)
  #include "core_cm3.h"
#elif defined (__CORTEX_M0)
  #include "core_cm0.h"
#else
  #error "Missing __CORTEX_Mx definition"
#endif

#include "rt_TypeDef.h"
#include "RTX_Config.h"
#include "rt_System.h"
#include "rt_Task.h"
#include "rt_Event.h"
#include "rt_List.h"
#include "rt_Time.h"
#include "rt_Mutex.h"
#include "rt_Semaphore.h"
#include "rt_Mailbox.h"
#include "rt_MemBox.h"
#include "rt_Memory.h"
#include "rt_HAL_CM.h"

#define os_thread_cb OS_TCB

#include "cmsis_os.h"

#if (osFeature_Signals != 16)
#error Invalid "osFeature_Signals" value!
#endif
#if (osFeature_Semaphore > 65535)
#error Invalid "osFeature_Semaphore" value!
#endif
#if (osFeature_Wait != 0)
#error osWait not supported!
#endif


// ==== Enumeration, structures, defines ====

// Service Calls defines

#if defined (__CC_ARM)          /* ARM Compiler */

#define __NO_RETURN __declspec(noreturn)

#define osEvent_type       osEvent
#define osEvent_ret_status ret
#define osEvent_ret_value  ret
#define osEvent_ret_msg    ret
#define osEvent_ret_mail   ret

#define osCallback_type    osCallback
#define osCallback_ret     ret

#define SVC_0_1(f,t,...)                                                       \
__svc_indirect(0) t  _##f (t(*)());                                            \
                  t     f (void);                                              \
__attribute__((always_inline))                                                 \
static __inline   t __##f (void) {                                             \
  return _##f(f);                                                              \
}

#define SVC_1_0(f,t,t1,...)                                                    \
__svc_indirect(0) t  _##f (t(*)(t1),t1);                                       \
                  t     f (t1 a1);                                             \
__attribute__((always_inline))                                                 \
static __inline   t __##f (t1 a1) {                                            \
  _##f(f,a1);                                                                  \
}

#define SVC_1_1(f,t,t1,...)                                                    \
__svc_indirect(0) t  _##f (t(*)(t1),t1);                                       \
                  t     f (t1 a1);                                             \
__attribute__((always_inline))                                                 \
static __inline   t __##f (t1 a1) {                                            \
  return _##f(f,a1);                                                           \
}

#define SVC_2_1(f,t,t1,t2,...)                                                 \
__svc_indirect(0) t  _##f (t(*)(t1,t2),t1,t2);                                 \
                  t     f (t1 a1, t2 a2);                                      \
__attribute__((always_inline))                                                 \
static __inline   t __##f (t1 a1, t2 a2) {                                     \
  return _##f(f,a1,a2);                                                        \
}

#define SVC_3_1(f,t,t1,t2,t3,...)                                              \
__svc_indirect(0) t  _##f (t(*)(t1,t2,t3),t1,t2,t3);                           \
                  t     f (t1 a1, t2 a2, t3 a3);                               \
__attribute__((always_inline))                                                 \
static __inline   t __##f (t1 a1, t2 a2, t3 a3) {                              \
  return _##f(f,a1,a2,a3);                                                     \
}

#define SVC_4_1(f,t,t1,t2,t3,t4,...)                                           \
__svc_indirect(0) t  _##f (t(*)(t1,t2,t3,t4),t1,t2,t3,t4);                     \
                  t     f (t1 a1, t2 a2, t3 a3, t4 a4);                        \
__attribute__((always_inline))                                                 \
static __inline   t __##f (t1 a1, t2 a2, t3 a3, t4 a4) {                       \
  return _##f(f,a1,a2,a3,a4);                                                  \
}

#define SVC_1_2 SVC_1_1 
#define SVC_1_3 SVC_1_1 
#define SVC_2_3 SVC_2_1 

#elif defined (__GNUC__)        /* GNU Compiler */

#define __NO_RETURN __attribute__((noreturn))

typedef uint32_t __attribute__((vector_size(8)))  ret64;
typedef uint32_t __attribute__((vector_size(16))) ret128;

#define RET_pointer    __r0
#define RET_int32_t    __r0
#define RET_uint32_t   __r0
#define RET_osStatus   __r0
#define RET_osPriority __r0
#define RET_osEvent    {(osStatus)__r0, {(uint32_t)__r1}, {(void *)__r2}}
#define RET_osCallback {(void *)__r0, (void *)__r1}

#define osEvent_type       __attribute__((pcs("aapcs"))) ret128
#define osEvent_ret_status (ret128){ret.status}
#define osEvent_ret_value  (ret128){ret.status, ret.value.v}
#define osEvent_ret_msg    (ret128){ret.status, ret.value.v, (uint32_t)ret.def.message_id}
#define osEvent_ret_mail   (ret128){ret.status, ret.value.v, (uint32_t)ret.def.mail_id}

#define osCallback_type    __attribute__((pcs("aapcs"))) ret64
#define osCallback_ret     (ret64) {(uint32_t)ret.fp, (uint32_t)ret.arg}

#define SVC_ArgN(n) \
  register int __r##n __asm("r"#n);

#define SVC_ArgR(n,t,a) \
  register t   __r##n __asm("r"#n) = a;

#define SVC_Arg0()                                                             \
  SVC_ArgN(0)                                                                  \
  SVC_ArgN(1)                                                                  \
  SVC_ArgN(2)                                                                  \
  SVC_ArgN(3)

#define SVC_Arg1(t1)                                                           \
  SVC_ArgR(0,t1,a1)                                                            \
  SVC_ArgN(1)                                                                  \
  SVC_ArgN(2)                                                                  \
  SVC_ArgN(3)

#define SVC_Arg2(t1,t2)                                                        \
  SVC_ArgR(0,t1,a1)                                                            \
  SVC_ArgR(1,t2,a2)                                                            \
  SVC_ArgN(2)                                                                  \
  SVC_ArgN(3)

#define SVC_Arg3(t1,t2,t3)                                                     \
  SVC_ArgR(0,t1,a1)                                                            \
  SVC_ArgR(1,t2,a2)                                                            \
  SVC_ArgR(2,t3,a3)                                                            \
  SVC_ArgN(3)

#define SVC_Arg4(t1,t2,t3,t4)                                                  \
  SVC_ArgR(0,t1,a1)                                                            \
  SVC_ArgR(1,t2,a2)                                                            \
  SVC_ArgR(2,t3,a3)                                                            \
  SVC_ArgR(3,t4,a4)

#if (defined (__CORTEX_M0))
#define SVC_Call(f)                                                            \
  __asm volatile                                                               \
  (                                                                            \
    "ldr r7,="#f"\n\t"                                                         \
    "mov r12,r7\n\t"                                                           \
    "svc 0"                                                                    \
    :               "=r" (__r0), "=r" (__r1), "=r" (__r2), "=r" (__r3)         \
    :                "r" (__r0),  "r" (__r1),  "r" (__r2),  "r" (__r3)         \
    : "r7", "r12", "lr", "cc"                                                  \
  );
#else
#define SVC_Call(f)                                                            \
  __asm volatile                                                               \
  (                                                                            \
    "ldr r12,="#f"\n\t"                                                        \
    "svc 0"                                                                    \
    :               "=r" (__r0), "=r" (__r1), "=r" (__r2), "=r" (__r3)         \
    :                "r" (__r0),  "r" (__r1),  "r" (__r2),  "r" (__r3)         \
    : "r12", "lr", "cc"                                                        \
  );
#endif

#define SVC_0_1(f,t,rv)                                                        \
__attribute__((always_inline))                                                 \
static inline  t __##f (void) {                                                \
  SVC_Arg0();                                                                  \
  SVC_Call(f);                                                                 \
  return (t) rv;                                                               \
}

#define SVC_1_0(f,t,t1)                                                        \
__attribute__((always_inline))                                                 \
static inline  t __##f (t1 a1) {                                               \
  SVC_Arg1(t1);                                                                \
  SVC_Call(f);                                                                 \
}

#define SVC_1_1(f,t,t1,rv)                                                     \
__attribute__((always_inline))                                                 \
static inline  t __##f (t1 a1) {                                               \
  SVC_Arg1(t1);                                                                \
  SVC_Call(f);                                                                 \
  return (t) rv;                                                               \
}

#define SVC_2_1(f,t,t1,t2,rv)                                                  \
__attribute__((always_inline))                                                 \
static inline  t __##f (t1 a1, t2 a2) {                                        \
  SVC_Arg2(t1,t2);                                                             \
  SVC_Call(f);                                                                 \
  return (t) rv;                                                               \
}

#define SVC_3_1(f,t,t1,t2,t3,rv)                                               \
__attribute__((always_inline))                                                 \
static inline  t __##f (t1 a1, t2 a2, t3 a3) {                                 \
  SVC_Arg3(t1,t2,t3);                                                          \
  SVC_Call(f);                                                                 \
  return (t) rv;                                                               \
}

#define SVC_4_1(f,t,t1,t2,t3,t4,rv)                                            \
__attribute__((always_inline))                                                 \
static inline  t __##f (t1 a1, t2 a2, t3 a3, t4 a4) {                          \
  SVC_Arg4(t1,t2,t3,t4);                                                       \
  SVC_Call(f);                                                                 \
  return (t) rv;                                                               \
}

#define SVC_1_2 SVC_1_1 
#define SVC_1_3 SVC_1_1 
#define SVC_2_3 SVC_2_1 

#elif defined (__ICCARM__)      /* IAR Compiler */

#define __NO_RETURN __noreturn

#define RET_osEvent        "=r"(ret.status), "=r"(ret.value), "=r"(ret.def)
#define RET_osCallback     "=r"(ret.fp), "=r"(ret.arg)

#define osEvent_type       osEvent
#define osEvent_ret_status ret
#define osEvent_ret_value  ret
#define osEvent_ret_msg    ret
#define osEvent_ret_mail   ret

#define osCallback_type    uint64_t
#define osCallback_ret     ((uint64_t)ret.fp | ((uint64_t)ret.arg)<<32)

#define SVC_Setup(f)                                                           \
  __asm(                                                                       \
    "mov r12,%0\n"                                                             \
    :: "r"(&f): "r12"                                                          \
  );

#define SVC_Ret3()                                                             \
  __asm(                                                                       \
    "ldr r0,[sp,#0]\n"                                                         \
    "ldr r1,[sp,#4]\n"                                                         \
    "ldr r2,[sp,#8]\n"                                                         \
  );

#define SVC_0_1(f,t,...)                                                       \
t f (void);                                                                    \
_Pragma("swi_number=0") __swi t _##f (void);                                   \
static inline t __##f (void) {                                                 \
  SVC_Setup(f);                                                                \
  return _##f();                                                               \
}

#define SVC_1_0(f,t,t1,...)                                                    \
t f (t1 a1);                                                                   \
_Pragma("swi_number=0") __swi t _##f (t1 a1);                                  \
static inline t __##f (t1 a1) {                                                \
  SVC_Setup(f);                                                                \
  _##f(a1);                                                                    \
}

#define SVC_1_1(f,t,t1,...)                                                    \
t f (t1 a1);                                                                   \
_Pragma("swi_number=0") __swi t _##f (t1 a1);                                  \
static inline t __##f (t1 a1) {                                                \
  SVC_Setup(f);                                                                \
  return _##f(a1);                                                             \
}

#define SVC_2_1(f,t,t1,t2,...)                                                 \
t f (t1 a1, t2 a2);                                                            \
_Pragma("swi_number=0") __swi t _##f (t1 a1, t2 a2);                           \
static inline t __##f (t1 a1, t2 a2) {                                         \
  SVC_Setup(f);                                                                \
  return _##f(a1,a2);                                                          \
}

#define SVC_3_1(f,t,t1,t2,t3,...)                                              \
t f (t1 a1, t2 a2, t3 a3);                                                     \
_Pragma("swi_number=0") __swi t _##f (t1 a1, t2 a2, t3 a3);                    \
static inline t __##f (t1 a1, t2 a2, t3 a3) {                                  \
  SVC_Setup(f);                                                                \
  return _##f(a1,a2,a3);                                                       \
}

#define SVC_4_1(f,t,t1,t2,t3,t4,...)                                           \
t f (t1 a1, t2 a2, t3 a3, t4 a4);                                              \
_Pragma("swi_number=0") __swi t _##f (t1 a1, t2 a2, t3 a3, t4 a4);             \
static inline t __##f (t1 a1, t2 a2, t3 a3, t4 a4) {                           \
  SVC_Setup(f);                                                                \
  return _##f(a1,a2,a3,a4);                                                    \
}

#define SVC_1_2(f,t,t1,rr)                                                     \
uint64_t f (t1 a1);                                                            \
_Pragma("swi_number=0") __swi uint64_t _##f (t1 a1);                           \
static inline t __##f (t1 a1) {                                                \
  t ret;                                                                       \
  SVC_Setup(f);                                                                \
  _##f(a1);                                                                    \
  __asm("" : rr : :);                                                          \
  return ret;                                                                  \
}

#define SVC_1_3(f,t,t1,rr)                                                     \
t f (t1 a1);                                                                   \
void f##_ (t1 a1) {                                                            \
  f(a1);                                                                       \
  SVC_Ret3();                                                                  \
}                                                                              \
_Pragma("swi_number=0") __swi void _##f (t1 a1);                               \
static inline t __##f (t1 a1) {                                                \
  t ret;                                                                       \
  SVC_Setup(f##_);                                                             \
  _##f(a1);                                                                    \
  __asm("" : rr : :);                                                          \
  return ret;                                                                  \
}

#define SVC_2_3(f,t,t1,t2,rr)                                                  \
t f (t1 a1, t2 a2);                                                            \
void f##_ (t1 a1, t2 a2) {                                                     \
  f(a1,a2);                                                                    \
  SVC_Ret3();                                                                  \
}                                                                              \
_Pragma("swi_number=0") __swi void _##f (t1 a1, t2 a2);                        \
static inline t __##f (t1 a1, t2 a2) {                                         \
  t ret;                                                                       \
  SVC_Setup(f##_);                                                             \
  _##f(a1,a2);                                                                 \
  __asm("" : rr : :);                                                          \
  return ret;                                                                  \
}

#endif


// Callback structure
typedef struct {
  void *fp;             // Function pointer
  void *arg;            // Function argument
} osCallback;


// OS Section definitions
#ifdef OS_SECTIONS_LINK_INFO
extern const uint32_t  os_section_id$$Base;
extern const uint32_t  os_section_id$$Limit;
#endif

// OS Stack Memory for Threads definitions
extern       uint64_t  os_stack_mem[];
extern const uint32_t  os_stack_sz;

// OS Timers external resources
extern const osThreadDef_t   os_thread_def_osTimerThread;
extern       osThreadId      osThreadId_osTimerThread;
extern const osMessageQDef_t os_messageQ_def_osTimerMessageQ;
extern       osMessageQId    osMessageQId_osTimerMessageQ;


// ==== Helper Functions ====

/// Convert timeout in millisec to system ticks
static uint16_t rt_ms2tick (uint32_t millisec) {
  uint32_t tick;

  if (millisec == 0U) { return 0x0U; }                  // No timeout
  if (millisec == osWaitForever) { return 0xFFFFU; }    // Indefinite timeout
  if (millisec > 4000000U) { return 0xFFFEU; }          // Max ticks supported

  tick = ((1000U * millisec) + os_clockrate - 1U)  / os_clockrate;
  if (tick > 0xFFFEU) { return 0xFFFEU; }
  
  return (uint16_t)tick;
}

/// Convert Thread ID to TCB pointer
static P_TCB rt_tid2ptcb (osThreadId thread_id) {
  P_TCB ptcb;

  if (thread_id == NULL) { return NULL; }

  if ((uint32_t)thread_id & 3U) { return NULL; }

#ifdef OS_SECTIONS_LINK_INFO
  if ((os_section_id$$Base != 0U) && (os_section_id$$Limit != 0U)) {
    if (thread_id  < (osThreadId)os_section_id$$Base)  { return NULL; }
    if (thread_id >= (osThreadId)os_section_id$$Limit) { return NULL; }
  }
#endif

  ptcb = thread_id;

  if (ptcb->cb_type != TCB) { return NULL; }

  return ptcb;
}

/// Convert ID pointer to Object pointer
static void *rt_id2obj (void *id) {

  if ((uint32_t)id & 3U) { return NULL; }

#ifdef OS_SECTIONS_LINK_INFO
  if ((os_section_id$$Base != 0U) && (os_section_id$$Limit != 0U)) {
    if (id  < (void *)os_section_id$$Base)  { return NULL; }
    if (id >= (void *)os_section_id$$Limit) { return NULL; }
  }
#endif

  return id;
}


// ==== Kernel Control ====

uint8_t os_initialized;                         // Kernel Initialized flag
uint8_t os_running;                             // Kernel Running flag

// Kernel Control Service Calls declarations
SVC_0_1(svcKernelInitialize, osStatus, RET_osStatus)
SVC_0_1(svcKernelStart,      osStatus, RET_osStatus)
SVC_0_1(svcKernelRunning,    int32_t,  RET_int32_t)
SVC_0_1(svcKernelSysTick,    uint32_t, RET_uint32_t)

static void  sysThreadError   (osStatus status);
osThreadId   svcThreadCreate  (const osThreadDef_t *thread_def, void *argument);
osMessageQId svcMessageCreate (const osMessageQDef_t *queue_def, osThreadId thread_id);

// Kernel Control Service Calls

/// Initialize the RTOS Kernel for creating objects
osStatus svcKernelInitialize (void) {
  uint32_t ret;

  if (os_initialized == 0U) {

    // Init Thread Stack Memory (must be 8-byte aligned)
    if (((uint32_t)os_stack_mem & 7U) != 0U) { return osErrorNoMemory; }
    ret = rt_init_mem(os_stack_mem, os_stack_sz);
    if (ret != 0U) { return osErrorNoMemory; }

    rt_sys_init();                              // RTX System Initialization
  }

  os_tsk.run->prio = 255U;                      // Highest priority

  if (os_initialized == 0U) {
    // Create OS Timers resources (Message Queue & Thread)
    osMessageQId_osTimerMessageQ = svcMessageCreate (&os_messageQ_def_osTimerMessageQ, NULL);
    osThreadId_osTimerThread = svcThreadCreate(&os_thread_def_osTimerThread, NULL);
  }

  sysThreadError(osOK);

  os_initialized = 1U;
  os_running = 0U;

  return osOK;
}

/// Start the RTOS Kernel
osStatus svcKernelStart (void) {

  if (os_running) { return osOK; }

  rt_tsk_prio(0U, os_tsk.run->prio_base);       // Restore priority
  if (os_tsk.run->task_id == 0xFFU) {           // Idle Thread
    __set_PSP(os_tsk.run->tsk_stack + (8U*4U)); // Setup PSP
  }
  if (os_tsk.next == NULL) {                    // Force context switch
    os_tsk.next = os_tsk.run;
    os_tsk.run  = NULL;
  }

  rt_sys_start();

  os_running = 1U;

  return osOK;
}

/// Check if the RTOS kernel is already started
int32_t svcKernelRunning (void) {
  return (int32_t)os_running;
}

/// Get the RTOS kernel system timer counter
uint32_t svcKernelSysTick (void) {
  uint32_t tick, tick0;

  tick = os_tick_val();
  if (os_tick_ovf()) {
    tick0 = os_tick_val();
    if (tick0 < tick) { tick = tick0; }
    tick += (os_trv + 1U) * (os_time + 1U);
  } else {
    tick += (os_trv + 1U) *  os_time;
  }

  return tick;
}

// Kernel Control Public API

/// Initialize the RTOS Kernel for creating objects
osStatus osKernelInitialize (void) {
  if (__get_IPSR() != 0U) {
    return osErrorISR;                          // Not allowed in ISR
  }
  if ((__get_CONTROL() & 1U) == 0U) {           // Privileged mode
    return   svcKernelInitialize();
  } else {
    return __svcKernelInitialize();
  }
}

/// Start the RTOS Kernel
osStatus osKernelStart (void) {
  uint32_t stack[8];

  if (__get_IPSR() != 0U) {
    return osErrorISR;                          // Not allowed in ISR
  }
  switch (__get_CONTROL() & 0x03U) {
    case 0x00U:                                 // Privileged Thread mode & MSP
      __set_PSP((uint32_t)(stack + 8));         // Initial PSP
      if (os_flags & 1U) {                       
        __set_CONTROL(0x02U);                   // Set Privileged Thread mode & PSP
      } else {
        __set_CONTROL(0x03U);                   // Set Unprivileged Thread mode & PSP
      }
      __DSB();
      __ISB();
      break;
    case 0x01U:                                 // Unprivileged Thread mode & MSP
      return osErrorOS;
    case 0x02U:                                 // Privileged Thread mode & PSP
      if ((os_flags & 1U) == 0U) {              // Unprivileged Thread mode requested
        __set_CONTROL(0x03U);                   // Set Unprivileged Thread mode & PSP
        __DSB();
        __ISB();
      }
      break;
    case 0x03U:                                 // Unprivileged Thread mode & PSP
      if  (os_flags & 1U) { return osErrorOS; } // Privileged Thread mode requested
      break;
  }
  return __svcKernelStart();
}

/// Check if the RTOS kernel is already started
int32_t osKernelRunning (void) {
  if ((__get_IPSR() != 0U) || ((__get_CONTROL() & 1U) == 0U)) {
    // in ISR or Privileged
    return (int32_t)os_running;
  } else {
    return __svcKernelRunning();
  }
}

/// Get the RTOS kernel system timer counter
uint32_t osKernelSysTick (void) {
  if (__get_IPSR() != 0U) { return 0U; }        // Not allowed in ISR
  return __svcKernelSysTick();
}


// ==== Thread Management ====

/// Set Thread Error (for Create functions which return IDs)
static void sysThreadError (osStatus status) {
  // To Do
}

__NO_RETURN void osThreadExit (void);

// Thread Service Calls declarations
SVC_2_1(svcThreadCreate,      osThreadId, const osThreadDef_t *, void *,     RET_pointer)
SVC_0_1(svcThreadGetId,       osThreadId,                                    RET_pointer)
SVC_1_1(svcThreadTerminate,   osStatus,         osThreadId,                  RET_osStatus)
SVC_0_1(svcThreadYield,       osStatus,                                      RET_osStatus)
SVC_2_1(svcThreadSetPriority, osStatus,         osThreadId,      osPriority, RET_osStatus)
SVC_1_1(svcThreadGetPriority, osPriority,       osThreadId,                  RET_osPriority)

// Thread Service Calls

/// Create a thread and add it to Active Threads and set it to state READY
osThreadId svcThreadCreate (const osThreadDef_t *thread_def, void *argument) {
  P_TCB  ptcb;
  OS_TID tsk;
  void  *stk;

  if ((thread_def == NULL) ||
      (thread_def->pthread == NULL) ||
      (thread_def->tpriority < osPriorityIdle) ||
      (thread_def->tpriority > osPriorityRealtime)) {
    sysThreadError(osErrorParameter); 
    return NULL; 
  }

  if (thread_def->stacksize != 0U) {            // Custom stack size
    stk = rt_alloc_mem(                         // Allocate stack
      os_stack_mem,
      thread_def->stacksize
    );
    if (stk == NULL) { 
      sysThreadError(osErrorNoMemory);          // Out of memory
      return NULL;
    }
  } else {                                      // Default stack size
    stk = NULL;
  }

  tsk = rt_tsk_create(                          // Create task
    (FUNCP)thread_def->pthread,                 // Task function pointer
    (uint32_t)
    (thread_def->tpriority-osPriorityIdle+1) |  // Task priority
    (thread_def->stacksize << 8),               // Task stack size in bytes
    stk,                                        // Pointer to task's stack
    argument                                    // Argument to the task
  );

  if (tsk == 0U) {                              // Invalid task ID
    if (stk != NULL) {
      rt_free_mem(os_stack_mem, stk);           // Free allocated stack
    }
    sysThreadError(osErrorNoMemory);            // Create task failed (Out of memory)
    return NULL;
  }

  ptcb = (P_TCB)os_active_TCB[tsk - 1U];        // TCB pointer

  *((uint32_t *)ptcb->tsk_stack + 13) = (uint32_t)osThreadExit;

  return ptcb;
}

/// Return the thread ID of the current running thread
osThreadId svcThreadGetId (void) {
  OS_TID tsk;

  tsk = rt_tsk_self();
  if (tsk == 0U) { return NULL; }
  return (P_TCB)os_active_TCB[tsk - 1U];
}

/// Terminate execution of a thread and remove it from ActiveThreads
osStatus svcThreadTerminate (osThreadId thread_id) {
  OS_RESULT res;
  P_TCB     ptcb;
  void     *stk;

  ptcb = rt_tid2ptcb(thread_id);                // Get TCB pointer
  if (ptcb == NULL) { 
    return osErrorParameter;
  }

  stk = ptcb->priv_stack ? ptcb->stack : NULL;  // Private stack

  res = rt_tsk_delete(ptcb->task_id);           // Delete task

  if (res == OS_R_NOK) {
    return osErrorResource;                     // Delete task failed
  }

  if (stk != NULL) {                            
    rt_free_mem(os_stack_mem, stk);             // Free private stack
  }

  return osOK;
}

/// Pass control to next thread that is in state READY
osStatus svcThreadYield (void) {
  rt_tsk_pass();                                // Pass control to next task
  return osOK;
}

/// Change priority of an active thread
osStatus svcThreadSetPriority (osThreadId thread_id, osPriority priority) {
  OS_RESULT res;
  P_TCB     ptcb;

  ptcb = rt_tid2ptcb(thread_id);                // Get TCB pointer
  if (ptcb == NULL) { 
    return osErrorParameter; 
  }

  if ((priority < osPriorityIdle) || (priority > osPriorityRealtime)) {
    return osErrorValue;
  }

  res = rt_tsk_prio(                            // Change task priority
    ptcb->task_id,                              // Task ID
    (uint8_t)(priority - osPriorityIdle + 1)    // New task priority
  );

  if (res == OS_R_NOK) {
    return osErrorResource;                     // Change task priority failed
  }

  return osOK;
}

/// Get current priority of an active thread
osPriority svcThreadGetPriority (osThreadId thread_id) {
  P_TCB ptcb;

  ptcb = rt_tid2ptcb(thread_id);                // Get TCB pointer
  if (ptcb == NULL) {
    return osPriorityError;
  }

  return (osPriority)(ptcb->prio - 1 + osPriorityIdle); 
}


// Thread Public API

/// Create a thread and add it to Active Threads and set it to state READY
osThreadId osThreadCreate (const osThreadDef_t *thread_def, void *argument) {
  if (__get_IPSR() != 0U) { 
    return NULL;                                // Not allowed in ISR
  }
  if (((__get_CONTROL() & 1U) == 0U) && (os_running == 0U)) {
    // Privileged and not running
    return   svcThreadCreate(thread_def, argument);
  } else {
    return __svcThreadCreate(thread_def, argument);
  }
}

/// Return the thread ID of the current running thread
osThreadId osThreadGetId (void) {
  if (__get_IPSR() != 0U) {
    return NULL;                                // Not allowed in ISR
  }
  return __svcThreadGetId();
}

/// Terminate execution of a thread and remove it from ActiveThreads
osStatus osThreadTerminate (osThreadId thread_id) {
  if (__get_IPSR() != 0U) { 
    return osErrorISR;                          // Not allowed in ISR
  }
  return __svcThreadTerminate(thread_id);
}

/// Pass control to next thread that is in state READY
osStatus osThreadYield (void) {
  if (__get_IPSR() != 0U) {
    return osErrorISR;                          // Not allowed in ISR
  }
  return __svcThreadYield();
}

/// Change priority of an active thread
osStatus osThreadSetPriority (osThreadId thread_id, osPriority priority) {
  if (__get_IPSR() != 0U) {
    return osErrorISR;                          // Not allowed in ISR
  }
  return __svcThreadSetPriority(thread_id, priority);
}

/// Get current priority of an active thread
osPriority osThreadGetPriority (osThreadId thread_id) {
  if (__get_IPSR() != 0U) {
    return osPriorityError;                     // Not allowed in ISR
  }
  return __svcThreadGetPriority(thread_id);
}

/// INTERNAL - Not Public
/// Auto Terminate Thread on exit (used implicitly when thread exists)
__NO_RETURN void osThreadExit (void) { 
  __svcThreadTerminate(__svcThreadGetId()); 
  for (;;);                                     // Should never come here
}


// ==== Generic Wait Functions ====

// Generic Wait Service Calls declarations
SVC_1_1(svcDelay,           osStatus, uint32_t, RET_osStatus)
#if osFeature_Wait != 0
SVC_1_3(svcWait,  os_InRegs osEvent,  uint32_t, RET_osEvent)
#endif

// Generic Wait Service Calls

/// Wait for Timeout (Time Delay)
osStatus svcDelay (uint32_t millisec) {
  if (millisec == 0U) { return osOK; }
  rt_dly_wait(rt_ms2tick(millisec));
  return osEventTimeout;
}

/// Wait for Signal, Message, Mail, or Timeout
#if osFeature_Wait != 0
os_InRegs osEvent_type svcWait (uint32_t millisec) {
  osEvent ret;

  if (millisec == 0U) {
    ret.status = osOK;
    return osEvent_ret_status;
  }

  /* To Do: osEventSignal, osEventMessage, osEventMail */
  rt_dly_wait(rt_ms2tick(millisec));
  ret.status = osEventTimeout;

  return osEvent_ret_status;
}
#endif


// Generic Wait API

/// Wait for Timeout (Time Delay)
osStatus osDelay (uint32_t millisec) {
  if (__get_IPSR() != 0U) {
    return osErrorISR;                          // Not allowed in ISR
  }
  return __svcDelay(millisec);
}

/// Wait for Signal, Message, Mail, or Timeout
os_InRegs osEvent osWait (uint32_t millisec) {
  osEvent ret;

#if osFeature_Wait == 0
  ret.status = osErrorOS;
  return ret;
#else
  if (__get_IPSR() != 0U) {                     // Not allowed in ISR
    ret.status = osErrorISR;
    return ret;
  }
  return __svcWait(millisec);
#endif
}


// ==== Timer Management ====

// Timer definitions
#define osTimerInvalid  0U
#define osTimerStopped  1U
#define osTimerRunning  2U

// Timer structures 

typedef struct os_timer_cb_ {                   // Timer Control Block
  struct os_timer_cb_ *next;                    // Pointer to next active Timer
  uint8_t             state;                    // Timer State
  uint8_t              type;                    // Timer Type (Periodic/One-shot)
  uint16_t         reserved;                    // Reserved
  uint32_t             tcnt;                    // Timer Delay Count
  uint32_t             icnt;                    // Timer Initial Count 
  void                 *arg;                    // Timer Function Argument
  const osTimerDef_t *timer;                    // Pointer to Timer definition
} os_timer_cb;

// Timer variables
os_timer_cb *os_timer_head;                     // Pointer to first active Timer


// Timer Helper Functions

// Insert Timer into the list sorted by time
static void rt_timer_insert (os_timer_cb *pt, uint32_t tcnt) {
  os_timer_cb *p, *prev;

  prev = NULL;
  p = os_timer_head;
  while (p != NULL) {
    if (tcnt < p->tcnt) { break; }
    tcnt -= p->tcnt;
    prev = p;
    p = p->next;
  }
  pt->next = p;
  pt->tcnt = tcnt;
  if (p != NULL) {
    p->tcnt -= pt->tcnt;
  }
  if (prev != NULL) {
    prev->next = pt;
  } else {
    os_timer_head = pt;
  }
}

// Remove Timer from the list
static int32_t rt_timer_remove (os_timer_cb *pt) {
  os_timer_cb *p, *prev;

  prev = NULL;
  p = os_timer_head;
  while (p != NULL) {
    if (p == pt) { break; }
    prev = p;
    p = p->next;
  }
  if (p == NULL) { return -1; }
  if (prev != NULL) {
    prev->next = pt->next;
  } else {
    os_timer_head = pt->next;
  }
  if (pt->next != NULL) {
    pt->next->tcnt += pt->tcnt;
  }

  return 0;
}


// Timer Service Calls declarations
SVC_3_1(svcTimerCreate,           osTimerId,  const osTimerDef_t *, os_timer_type, void *, RET_pointer)
SVC_2_1(svcTimerStart,            osStatus,         osTimerId,      uint32_t,              RET_osStatus)
SVC_1_1(svcTimerStop,             osStatus,         osTimerId,                             RET_osStatus)
SVC_1_1(svcTimerDelete,           osStatus,         osTimerId,                             RET_osStatus)
SVC_1_2(svcTimerCall,   os_InRegs osCallback,       osTimerId,                             RET_osCallback)

// Timer Management Service Calls

/// Create timer
osTimerId svcTimerCreate (const osTimerDef_t *timer_def, os_timer_type type, void *argument) {
  os_timer_cb *pt;

  if ((timer_def == NULL) || (timer_def->ptimer == NULL)) {
    sysThreadError(osErrorParameter);
    return NULL;
  }

  pt = timer_def->timer;
  if (pt == NULL) {
    sysThreadError(osErrorParameter);
    return NULL;
  }

  if ((type != osTimerOnce) && (type != osTimerPeriodic)) {
    sysThreadError(osErrorValue);
    return NULL;
  }

  if (osThreadId_osTimerThread == NULL) {
    sysThreadError(osErrorResource);
    return NULL;
  }

  if (pt->state != osTimerInvalid){
    sysThreadError(osErrorResource);
    return NULL;
  }

  pt->next  = NULL;
  pt->state = osTimerStopped;
  pt->type  =  (uint8_t)type;
  pt->arg   = argument;
  pt->timer = timer_def;

  return (osTimerId)pt;
}

/// Start or restart timer
osStatus svcTimerStart (osTimerId timer_id, uint32_t millisec) {
  os_timer_cb *pt;
  uint32_t     tcnt;

  pt = rt_id2obj(timer_id);
  if (pt == NULL) {
    return osErrorParameter;
  }

  if (millisec == 0U) { return osErrorValue; }

  tcnt = (uint32_t)(((1000U * (uint64_t)millisec) + os_clockrate - 1U)  / os_clockrate);

  switch (pt->state) {
    case osTimerRunning:
      if (rt_timer_remove(pt) != 0) {
        return osErrorResource;
      }
      break;
    case osTimerStopped:
      pt->state = osTimerRunning;
      pt->icnt  = tcnt;
      break;
    default:
      return osErrorResource;
  }
  
  rt_timer_insert(pt, tcnt);

  return osOK;
}

/// Stop timer
osStatus svcTimerStop (osTimerId timer_id) {
  os_timer_cb *pt;

  pt = rt_id2obj(timer_id);
  if (pt == NULL) {
    return osErrorParameter;
  }

  if (pt->state != osTimerRunning) { return osErrorResource; }

  pt->state = osTimerStopped;

  if (rt_timer_remove(pt) != 0) {
    return osErrorResource;
  }

  return osOK;
}

/// Delete timer
osStatus svcTimerDelete (osTimerId timer_id) {
  os_timer_cb *pt;

  pt = rt_id2obj(timer_id);
  if (pt == NULL) {
    return osErrorParameter;
  }

  switch (pt->state) {
    case osTimerRunning:
      rt_timer_remove(pt);
      break;
    case osTimerStopped:
      break;
    default:
      return osErrorResource;
  }

  pt->state = osTimerInvalid;

  return osOK;
}

/// Get timer callback parameters
os_InRegs osCallback_type svcTimerCall (osTimerId timer_id) {
  os_timer_cb *pt;
  osCallback   ret;

  pt = rt_id2obj(timer_id);
  if (pt == NULL) {
    ret.fp  = NULL;
    ret.arg = NULL;
    return osCallback_ret;
  }

  ret.fp  = (void *)pt->timer->ptimer;
  ret.arg = pt->arg;

  return osCallback_ret;
}

osStatus isrMessagePut (osMessageQId queue_id, uint32_t info, uint32_t millisec);

/// Timer Tick (called each SysTick)
void sysTimerTick (void) {
  os_timer_cb *pt, *p;
  osStatus     status;

  p = os_timer_head;
  if (p == NULL) { return; }

  p->tcnt--;
  while ((p != NULL) && (p->tcnt == 0U)) {
    pt = p;
    p = p->next;
    os_timer_head = p;
    status = isrMessagePut(osMessageQId_osTimerMessageQ, (uint32_t)pt, 0U);
    if (status != osOK) {
      os_error(OS_ERR_TIMER_OVF);
    }
    if (pt->type == (uint8_t)osTimerPeriodic) {
      rt_timer_insert(pt, pt->icnt);
    } else {
      pt->state = osTimerStopped;
    }
  }
}

/// Get user timers wake-up time 
uint32_t sysUserTimerWakeupTime (void) {

  if (os_timer_head) {
    return os_timer_head->tcnt;
  }
  return 0xFFFFFFFFU;
}

/// Update user timers on resume
void sysUserTimerUpdate (uint32_t sleep_time) {

  while ((os_timer_head != NULL) && (sleep_time != 0U)) {
    if (sleep_time >= os_timer_head->tcnt) {
      sleep_time -= os_timer_head->tcnt;
      os_timer_head->tcnt = 1U;
      sysTimerTick();
    } else {
      os_timer_head->tcnt -= sleep_time;
      break;
    }
  }
}


// Timer Management Public API

/// Create timer
osTimerId osTimerCreate (const osTimerDef_t *timer_def, os_timer_type type, void *argument) {
  if (__get_IPSR() != 0U) {
    return NULL;                                // Not allowed in ISR
  }
  if (((__get_CONTROL() & 1U) == 0U) && (os_running == 0U)) {
    // Privileged and not running
    return   svcTimerCreate(timer_def, type, argument);
  } else {
    return __svcTimerCreate(timer_def, type, argument);
  }
}

/// Start or restart timer
osStatus osTimerStart (osTimerId timer_id, uint32_t millisec) {
  if (__get_IPSR() != 0U) {
    return osErrorISR;                          // Not allowed in ISR
  }
  return __svcTimerStart(timer_id, millisec);
}

/// Stop timer
osStatus osTimerStop (osTimerId timer_id) {
  if (__get_IPSR() != 0U) {
    return osErrorISR;                          // Not allowed in ISR
  }
  return __svcTimerStop(timer_id);
}

/// Delete timer
osStatus osTimerDelete (osTimerId timer_id) {
  if (__get_IPSR() != 0U) {
    return osErrorISR;                          // Not allowed in ISR
  }
  return __svcTimerDelete(timer_id);
}

/// INTERNAL - Not Public
/// Get timer callback parameters (used by OS Timer Thread)
os_InRegs osCallback osTimerCall (osTimerId timer_id) { 
  return __svcTimerCall(timer_id); 
}


// Timer Thread
__NO_RETURN void osTimerThread (void const *argument) {
  osCallback cb;
  osEvent    evt;

  for (;;) {
    evt = osMessageGet(osMessageQId_osTimerMessageQ, osWaitForever);
    if (evt.status == osEventMessage) {
      cb = osTimerCall(evt.value.p);
      if (cb.fp != NULL) {
        (*(os_ptimer)cb.fp)(cb.arg);
      }
    }
  }
}


// ==== Signal Management ====

// Signal Service Calls declarations
SVC_2_1(svcSignalSet,             int32_t, osThreadId, int32_t,  RET_int32_t)
SVC_2_1(svcSignalClear,           int32_t, osThreadId, int32_t,  RET_int32_t)
SVC_2_3(svcSignalWait,  os_InRegs osEvent, int32_t,    uint32_t, RET_osEvent)

// Signal Service Calls

/// Set the specified Signal Flags of an active thread
int32_t svcSignalSet (osThreadId thread_id, int32_t signals) {
  P_TCB   ptcb;
  int32_t sig;

  ptcb = rt_tid2ptcb(thread_id);                // Get TCB pointer
  if (ptcb == NULL) {
    return (int32_t)0x80000000U;
  }

  if ((uint32_t)signals & (0xFFFFFFFFU << osFeature_Signals)) {
    return (int32_t)0x80000000U;
  }

  sig = (int32_t)ptcb->events;                  // Previous signal flags

  rt_evt_set((uint16_t)signals, ptcb->task_id); // Set event flags

  return sig;
}

/// Clear the specified Signal Flags of an active thread
int32_t svcSignalClear (osThreadId thread_id, int32_t signals) {
  P_TCB   ptcb;
  int32_t sig;

  ptcb = rt_tid2ptcb(thread_id);                // Get TCB pointer
  if (ptcb == NULL) {
    return (int32_t)0x80000000U;
  }

  if ((uint32_t)signals & (0xFFFFFFFFU << osFeature_Signals)) {
    return (int32_t)0x80000000U;
  }

  sig = (int32_t)ptcb->events;                  // Previous signal flags

  rt_evt_clr((uint16_t)signals, ptcb->task_id); // Clear event flags

  return sig;
}

/// Wait for one or more Signal Flags to become signaled for the current RUNNING thread
os_InRegs osEvent_type svcSignalWait (int32_t signals, uint32_t millisec) {
  OS_RESULT res;
  osEvent   ret;

  if ((uint32_t)signals & (0xFFFFFFFFU << osFeature_Signals)) {
    ret.status = osErrorValue;
    return osEvent_ret_status;
  }

  if (signals != 0) {                           // Wait for all specified signals
    res = rt_evt_wait((uint16_t)signals, rt_ms2tick(millisec), __TRUE);
  } else {                                      // Wait for any signal
    res = rt_evt_wait(0xFFFFU,           rt_ms2tick(millisec), __FALSE);
  }

  if (res == OS_R_EVT) {
    ret.status = osEventSignal;
    ret.value.signals = (signals != 0) ? signals : (int32_t)os_tsk.run->waits;
  } else {
    ret.status = (millisec != 0U) ? osEventTimeout : osOK;
    ret.value.signals = 0;
  }

  return osEvent_ret_value;
}


// Signal ISR Calls

/// Set the specified Signal Flags of an active thread
int32_t isrSignalSet (osThreadId thread_id, int32_t signals) {
  P_TCB   ptcb;
  int32_t sig;

  ptcb = rt_tid2ptcb(thread_id);                // Get TCB pointer
  if (ptcb == NULL) {
    return (int32_t)0x80000000U;
  }

  if ((uint32_t)signals & (0xFFFFFFFFU << osFeature_Signals)) {
    return (int32_t)0x80000000U;
  }

  sig = (int32_t)ptcb->events;                  // Previous signal flags

  isr_evt_set((uint16_t)signals, ptcb->task_id);// Set event flags

  return sig;
}


// Signal Public API

/// Set the specified Signal Flags of an active thread
int32_t osSignalSet (osThreadId thread_id, int32_t signals) {
  if (__get_IPSR() != 0U) {                     // in ISR
    return   isrSignalSet(thread_id, signals); 
  } else {                                      // in Thread
    return __svcSignalSet(thread_id, signals);
  }
}

/// Clear the specified Signal Flags of an active thread
int32_t osSignalClear (osThreadId thread_id, int32_t signals) {
  if (__get_IPSR() != 0U) {
    return (int32_t)0x80000000U;                // Not allowed in ISR
  }
  return __svcSignalClear(thread_id, signals);
}

/// Wait for one or more Signal Flags to become signaled for the current RUNNING thread
os_InRegs osEvent osSignalWait (int32_t signals, uint32_t millisec) {
  osEvent ret;

  if (__get_IPSR() != 0U) {                     // Not allowed in ISR
    ret.status = osErrorISR;
    return ret;
  }
  return __svcSignalWait(signals, millisec);
}


// ==== Mutex Management ====

// Mutex Service Calls declarations
SVC_1_1(svcMutexCreate,  osMutexId, const osMutexDef_t *,           RET_pointer)
SVC_2_1(svcMutexWait,    osStatus,        osMutexId,      uint32_t, RET_osStatus)
SVC_1_1(svcMutexRelease, osStatus,        osMutexId,                RET_osStatus)
SVC_1_1(svcMutexDelete,  osStatus,        osMutexId,                RET_osStatus)

// Mutex Service Calls

/// Create and Initialize a Mutex object
osMutexId svcMutexCreate (const osMutexDef_t *mutex_def) {
  OS_ID mut;

  if (mutex_def == NULL) {
    sysThreadError(osErrorParameter);
    return NULL;
  }

  mut = mutex_def->mutex;
  if (mut == NULL) {
    sysThreadError(osErrorParameter);
    return NULL;
  }

  if (((P_MUCB)mut)->cb_type != 0U) {
    sysThreadError(osErrorParameter);
    return NULL;
  }

  rt_mut_init(mut);                             // Initialize Mutex

  return mut;
}

/// Wait until a Mutex becomes available
osStatus svcMutexWait (osMutexId mutex_id, uint32_t millisec) {
  OS_ID     mut;
  OS_RESULT res;

  mut = rt_id2obj(mutex_id);
  if (mut == NULL) {
    return osErrorParameter;
  }

  if (((P_MUCB)mut)->cb_type != MUCB) {
    return osErrorParameter;
  }

  res = rt_mut_wait(mut, rt_ms2tick(millisec)); // Wait for Mutex

  if (res == OS_R_TMO) {
    return ((millisec != 0U) ? osErrorTimeoutResource : osErrorResource);
  }
  if (res == OS_R_NOK) {
    return osErrorResource;
  }

  return osOK;
}

/// Release a Mutex that was obtained with osMutexWait
osStatus svcMutexRelease (osMutexId mutex_id) {
  OS_ID     mut;
  OS_RESULT res;

  mut = rt_id2obj(mutex_id);
  if (mut == NULL) {
    return osErrorParameter;
  }

  if (((P_MUCB)mut)->cb_type != MUCB) {
    return osErrorParameter;
  }

  res = rt_mut_release(mut);                    // Release Mutex

  if (res == OS_R_NOK) {
    return osErrorResource;                     // Thread not owner or Zero Counter
  }

  return osOK;
}

/// Delete a Mutex that was created by osMutexCreate
osStatus svcMutexDelete (osMutexId mutex_id) {
  OS_ID mut;

  mut = rt_id2obj(mutex_id);
  if (mut == NULL) {
    return osErrorParameter;
  }

  if (((P_MUCB)mut)->cb_type != MUCB) {
    return osErrorParameter;
  }

  rt_mut_delete(mut);                           // Release Mutex

  return osOK;
}


// Mutex Public API

/// Create and Initialize a Mutex object
osMutexId osMutexCreate (const osMutexDef_t *mutex_def) {
  if (__get_IPSR() != 0U) {
    return NULL;                                // Not allowed in ISR
  }
  if (((__get_CONTROL() & 1U) == 0U) && (os_running == 0U)) {
    // Privileged and not running
    return    svcMutexCreate(mutex_def);
  } else {
    return __svcMutexCreate(mutex_def);
  }
}

/// Wait until a Mutex becomes available
osStatus osMutexWait (osMutexId mutex_id, uint32_t millisec) {
  if (__get_IPSR() != 0U) {
    return osErrorISR;                          // Not allowed in ISR
  }
  return __svcMutexWait(mutex_id, millisec);
}

/// Release a Mutex that was obtained with osMutexWait
osStatus osMutexRelease (osMutexId mutex_id) {
  if (__get_IPSR() != 0U) {
    return osErrorISR;                          // Not allowed in ISR
  }
  return __svcMutexRelease(mutex_id);
}

/// Delete a Mutex that was created by osMutexCreate
osStatus osMutexDelete (osMutexId mutex_id) {
  if (__get_IPSR() != 0U) {
    return osErrorISR;                          // Not allowed in ISR
  }
  return __svcMutexDelete(mutex_id);
}


// ==== Semaphore Management ====

// Semaphore Service Calls declarations
SVC_2_1(svcSemaphoreCreate,  osSemaphoreId, const osSemaphoreDef_t *,  int32_t, RET_pointer)
SVC_2_1(svcSemaphoreWait,    int32_t,             osSemaphoreId,      uint32_t, RET_int32_t)
SVC_1_1(svcSemaphoreRelease, osStatus,            osSemaphoreId,                RET_osStatus)
SVC_1_1(svcSemaphoreDelete,  osStatus,            osSemaphoreId,                RET_osStatus)

// Semaphore Service Calls

/// Create and Initialize a Semaphore object
osSemaphoreId svcSemaphoreCreate (const osSemaphoreDef_t *semaphore_def, int32_t count) {
  OS_ID sem;

  if (semaphore_def == NULL) {
    sysThreadError(osErrorParameter);
    return NULL;
  }

  sem = semaphore_def->semaphore;
  if (sem == NULL) {
    sysThreadError(osErrorParameter);
    return NULL;
  }

  if (((P_SCB)sem)->cb_type != 0U) {
    sysThreadError(osErrorParameter);
    return NULL;
  }

  if (count > osFeature_Semaphore) {
    sysThreadError(osErrorValue);
    return NULL;
  }

  rt_sem_init(sem, (uint16_t)count);            // Initialize Semaphore
  
  return sem;
}

/// Wait until a Semaphore becomes available
int32_t svcSemaphoreWait (osSemaphoreId semaphore_id, uint32_t millisec) {
  OS_ID     sem;
  OS_RESULT res;

  sem = rt_id2obj(semaphore_id);
  if (sem == NULL) {
    return -1;
  }

  if (((P_SCB)sem)->cb_type != SCB) {
    return -1;
  }

  res = rt_sem_wait(sem, rt_ms2tick(millisec)); // Wait for Semaphore

  if (res == OS_R_TMO) { return 0; }            // Timeout

  return (int32_t)(((P_SCB)sem)->tokens + 1U);
}

/// Release a Semaphore
osStatus svcSemaphoreRelease (osSemaphoreId semaphore_id) {
  OS_ID sem;

  sem = rt_id2obj(semaphore_id);
  if (sem == NULL) {
    return osErrorParameter;
  }

  if (((P_SCB)sem)->cb_type != SCB) {
    return osErrorParameter;
  }

  if ((int32_t)((P_SCB)sem)->tokens == osFeature_Semaphore) {
    return osErrorResource;
  }
  
  rt_sem_send(sem);                             // Release Semaphore

  return osOK;
}

/// Delete a Semaphore that was created by osSemaphoreCreate
osStatus svcSemaphoreDelete (osSemaphoreId semaphore_id) {
  OS_ID sem;

  sem = rt_id2obj(semaphore_id);
  if (sem == NULL) {
    return osErrorParameter;
  }

  if (((P_SCB)sem)->cb_type != SCB) {
    return osErrorParameter;
  }

  rt_sem_delete(sem);                           // Delete Semaphore

  return osOK;
}


// Semaphore ISR Calls

/// Release a Semaphore
osStatus isrSemaphoreRelease (osSemaphoreId semaphore_id) {
  OS_ID sem;

  sem = rt_id2obj(semaphore_id);
  if (sem == NULL) {
    return osErrorParameter;
  }

  if (((P_SCB)sem)->cb_type != SCB) {
    return osErrorParameter;
  }

  if ((int32_t)((P_SCB)sem)->tokens == osFeature_Semaphore) {
    return osErrorResource;
  }

  isr_sem_send(sem);                            // Release Semaphore

  return osOK;
}


// Semaphore Public API

/// Create and Initialize a Semaphore object
osSemaphoreId osSemaphoreCreate (const osSemaphoreDef_t *semaphore_def, int32_t count) {
  if (__get_IPSR() != 0U) {
    return NULL;                                // Not allowed in ISR
  }
  if (((__get_CONTROL() & 1U) == 0U) && (os_running == 0U)) {
    // Privileged and not running
    return   svcSemaphoreCreate(semaphore_def, count);
  } else {
    return __svcSemaphoreCreate(semaphore_def, count);
  }
}

/// Wait until a Semaphore becomes available
int32_t osSemaphoreWait (osSemaphoreId semaphore_id, uint32_t millisec) {
  if (__get_IPSR() != 0U) {
    return -1;                                  // Not allowed in ISR
  }
  return __svcSemaphoreWait(semaphore_id, millisec);
}

/// Release a Semaphore
osStatus osSemaphoreRelease (osSemaphoreId semaphore_id) {
  if (__get_IPSR() != 0U) {                     // in ISR
    return   isrSemaphoreRelease(semaphore_id);
  } else {                                      // in Thread
    return __svcSemaphoreRelease(semaphore_id);
  }
}

/// Delete a Semaphore that was created by osSemaphoreCreate
osStatus osSemaphoreDelete (osSemaphoreId semaphore_id) {
  if (__get_IPSR() != 0U) {
    return osErrorISR;                          // Not allowed in ISR
  }
  return __svcSemaphoreDelete(semaphore_id);
}


// ==== Memory Management Functions ====

// Memory Management Helper Functions

// Clear Memory Box (Zero init)
static void rt_clr_box (void *box_mem, void *box) {
  uint32_t *p, n;

  if ((box_mem != NULL) && (box != NULL)) {
    p = box;
    for (n = ((P_BM)box_mem)->blk_size; n; n -= 4U) {
      *p++ = 0U;
    }
  }
}

// Memory Management Service Calls declarations
SVC_1_1(svcPoolCreate, osPoolId, const osPoolDef_t *,         RET_pointer)
SVC_1_1(sysPoolAlloc,  void *,         osPoolId,              RET_pointer)
SVC_2_1(sysPoolFree,   osStatus,       osPoolId,      void *, RET_osStatus)

// Memory Management Service & ISR Calls

/// Create and Initialize memory pool
osPoolId svcPoolCreate (const osPoolDef_t *pool_def) {
  uint32_t blk_sz;

  if ((pool_def == NULL) ||
      (pool_def->pool_sz == 0U) ||
      (pool_def->item_sz == 0U) ||
      (pool_def->pool == NULL)) {
    sysThreadError(osErrorParameter);
    return NULL;
  }

  blk_sz = (pool_def->item_sz + 3U) & (uint32_t)~3U;

  _init_box(pool_def->pool, sizeof(struct OS_BM) + (pool_def->pool_sz * blk_sz), blk_sz);

  return pool_def->pool;
}

/// Allocate a memory block from a memory pool
void *sysPoolAlloc (osPoolId pool_id) {
  void *mem;

  if (pool_id == NULL) {
    return NULL;
  }

  mem = rt_alloc_box(pool_id);

  return mem;
}

/// Return an allocated memory block back to a specific memory pool
osStatus sysPoolFree (osPoolId pool_id, void *block) {
  uint32_t res;
    
  if (pool_id == NULL) {
    return osErrorParameter;
  }

  res = rt_free_box(pool_id, block);
  if (res != 0) {
    return osErrorValue;
  }

  return osOK;
}


// Memory Management Public API

/// Create and Initialize memory pool
osPoolId osPoolCreate (const osPoolDef_t *pool_def) {
  if (__get_IPSR() != 0U) {
    return NULL;                                // Not allowed in ISR
  }
  if (((__get_CONTROL() & 1U) == 0U) && (os_running == 0U)) {
    // Privileged and not running
    return   svcPoolCreate(pool_def);
  } else {
    return __svcPoolCreate(pool_def);
  }
}

/// Allocate a memory block from a memory pool
void *osPoolAlloc (osPoolId pool_id) {
  if ((__get_IPSR() != 0U) || ((__get_CONTROL() & 1U) == 0U)) {     // in ISR or Privileged
    return   sysPoolAlloc(pool_id);
  } else {                                      // in Thread
    return __sysPoolAlloc(pool_id);
  }
}

/// Allocate a memory block from a memory pool and set memory block to zero
void *osPoolCAlloc (osPoolId pool_id) {
  void *mem;

  if ((__get_IPSR() != 0U) || ((__get_CONTROL() & 1U) == 0U)) {     // in ISR or Privileged
    mem =   sysPoolAlloc(pool_id);
  } else {                                      // in Thread
    mem = __sysPoolAlloc(pool_id);
  }

  rt_clr_box(pool_id, mem);

  return mem;
}

/// Return an allocated memory block back to a specific memory pool
osStatus osPoolFree (osPoolId pool_id, void *block) {
  if ((__get_IPSR() != 0U) || ((__get_CONTROL() & 1U) == 0U)) {     // in ISR or Privileged
    return   sysPoolFree(pool_id, block);
  } else {                                      // in Thread
    return __sysPoolFree(pool_id, block);
  }
}


// ==== Message Queue Management Functions ====

// Message Queue Management Service Calls declarations
SVC_2_1(svcMessageCreate,        osMessageQId, const osMessageQDef_t *, osThreadId,           RET_pointer)
SVC_3_1(svcMessagePut,           osStatus,           osMessageQId,      uint32_t,   uint32_t, RET_osStatus)
SVC_2_3(svcMessageGet, os_InRegs osEvent,            osMessageQId,      uint32_t,             RET_osEvent)

// Message Queue Service Calls

/// Create and Initialize Message Queue
osMessageQId svcMessageCreate (const osMessageQDef_t *queue_def, osThreadId thread_id) {

  if ((queue_def == NULL) ||
      (queue_def->queue_sz == 0U) ||
      (queue_def->pool == NULL)) {
    sysThreadError(osErrorParameter);
    return NULL;
  }
  
  if (((P_MCB)queue_def->pool)->cb_type != 0U) {
    sysThreadError(osErrorParameter);
    return NULL;
  }

  rt_mbx_init(queue_def->pool, (uint16_t)(4U*(queue_def->queue_sz + 4U)));

  return queue_def->pool;
}

/// Put a Message to a Queue
osStatus svcMessagePut (osMessageQId queue_id, uint32_t info, uint32_t millisec) {
  OS_RESULT res;

  if (queue_id == NULL) {
    return osErrorParameter;
  }

  if (((P_MCB)queue_id)->cb_type != MCB) {
    return osErrorParameter;
  }

  res = rt_mbx_send(queue_id, (void *)info, rt_ms2tick(millisec));

  if (res == OS_R_TMO) {
    return ((millisec != 0U) ? osErrorTimeoutResource : osErrorResource);
  }

  return osOK;
}

/// Get a Message or Wait for a Message from a Queue
os_InRegs osEvent_type svcMessageGet (osMessageQId queue_id, uint32_t millisec) {
  OS_RESULT res;
  osEvent   ret;

  if (queue_id == NULL) {
    ret.status = osErrorParameter;
    return osEvent_ret_status;
  }

  if (((P_MCB)queue_id)->cb_type != MCB) {
    ret.status = osErrorParameter;
    return osEvent_ret_status;
  }

  res = rt_mbx_wait(queue_id, &ret.value.p, rt_ms2tick(millisec));
  
  if (res == OS_R_TMO) {
    ret.status = (millisec != 0U) ? osEventTimeout : osOK;
    return osEvent_ret_value;
  }

  ret.status = osEventMessage;

  return osEvent_ret_value;
}


// Message Queue ISR Calls

/// Put a Message to a Queue
osStatus isrMessagePut (osMessageQId queue_id, uint32_t info, uint32_t millisec) {

  if ((queue_id == NULL) || (millisec != 0U)) {
    return osErrorParameter;
  }

  if (((P_MCB)queue_id)->cb_type != MCB) {
    return osErrorParameter;
  }

  if (rt_mbx_check(queue_id) == 0U) {           // Check if Queue is full
    return osErrorResource;
  }

  isr_mbx_send(queue_id, (void *)info);

  return osOK;
}

/// Get a Message or Wait for a Message from a Queue
os_InRegs osEvent isrMessageGet (osMessageQId queue_id, uint32_t millisec) {
  OS_RESULT res;
  osEvent   ret;

  if ((queue_id == NULL) || (millisec != 0U)) {
    ret.status = osErrorParameter;
    return ret;
  }

  if (((P_MCB)queue_id)->cb_type != MCB) {
    ret.status = osErrorParameter;
    return ret;
  }

  res = isr_mbx_receive(queue_id, &ret.value.p);
  
  if (res != OS_R_MBX) {
    ret.status = osOK;
    return ret;
  }

  ret.status = osEventMessage; 

  return ret;
}


// Message Queue Management Public API

/// Create and Initialize Message Queue
osMessageQId osMessageCreate (const osMessageQDef_t *queue_def, osThreadId thread_id) {
  if (__get_IPSR() != 0U) {
    return NULL;                                // Not allowed in ISR
  }
  if (((__get_CONTROL() & 1U) == 0U) && (os_running == 0U)) {
    // Privileged and not running
    return   svcMessageCreate(queue_def, thread_id);
  } else {
    return __svcMessageCreate(queue_def, thread_id);
  }
}

/// Put a Message to a Queue
osStatus osMessagePut (osMessageQId queue_id, uint32_t info, uint32_t millisec) {
  if (__get_IPSR() != 0U) {                     // in ISR
    return   isrMessagePut(queue_id, info, millisec);
  } else {                                      // in Thread
    return __svcMessagePut(queue_id, info, millisec);
  }
}

/// Get a Message or Wait for a Message from a Queue
os_InRegs osEvent osMessageGet (osMessageQId queue_id, uint32_t millisec) {
  if (__get_IPSR() != 0U) {                     // in ISR
    return   isrMessageGet(queue_id, millisec);
  } else {                                      // in Thread
    return __svcMessageGet(queue_id, millisec);
  }
}


// ==== Mail Queue Management Functions ====

// Mail Queue Management Service Calls declarations
SVC_2_1(svcMailCreate, osMailQId, const osMailQDef_t *, osThreadId,         RET_pointer)
SVC_3_1(sysMailAlloc,  void *,          osMailQId,      uint32_t, uint32_t, RET_pointer)
SVC_3_1(sysMailFree,   osStatus,        osMailQId,      void *,   uint32_t, RET_osStatus)

// Mail Queue Management Service & ISR Calls

/// Create and Initialize mail queue
osMailQId svcMailCreate (const osMailQDef_t *queue_def, osThreadId thread_id) {
  uint32_t blk_sz;
  P_MCB    pmcb;
  void    *pool;

  if ((queue_def == NULL) ||
      (queue_def->queue_sz == 0U) ||
      (queue_def->item_sz  == 0U) ||
      (queue_def->pool == NULL)) {
    sysThreadError(osErrorParameter);
    return NULL;
  }

  pmcb = *(((void **)queue_def->pool) + 0);
  pool = *(((void **)queue_def->pool) + 1);

  if ((pool == NULL) || (pmcb == NULL) || (pmcb->cb_type != 0U)) {
    sysThreadError(osErrorParameter);
    return NULL;
  }

  blk_sz = (queue_def->item_sz + 3U) & (uint32_t)~3U;

  _init_box(pool, sizeof(struct OS_BM) + (queue_def->queue_sz * blk_sz), blk_sz);

  rt_mbx_init(pmcb, (uint16_t)(4U*(queue_def->queue_sz + 4U)));

  return queue_def->pool;
}

/// Allocate a memory block from a mail
void *sysMailAlloc (osMailQId queue_id, uint32_t millisec, uint32_t isr) {
  P_MCB pmcb;
  void *pool;
  void *mem;

  if (queue_id == NULL) {
    return NULL;
  }

  pmcb = *(((void **)queue_id) + 0);
  pool = *(((void **)queue_id) + 1);

  if ((pool == NULL) || (pmcb == NULL)) {
    return NULL; 
  }

  if ((isr != 0U) && (millisec != 0U)) {
    return NULL;
  }

  mem = rt_alloc_box(pool);

  if ((mem == NULL) && (millisec != 0U)) {
    // Put Task to sleep when Memory not available
    if (pmcb->p_lnk != NULL) {
      rt_put_prio((P_XCB)pmcb, os_tsk.run);
    } else {
      pmcb->p_lnk = os_tsk.run;
      os_tsk.run->p_lnk = NULL;
      os_tsk.run->p_rlnk = (P_TCB)pmcb;
      // Task is waiting to allocate a message
      pmcb->state = 3U;
    }
    rt_block(rt_ms2tick(millisec), WAIT_MBX);
  }

  return mem;  
}

/// Free a memory block from a mail
osStatus sysMailFree (osMailQId queue_id, void *mail, uint32_t isr) {
  P_MCB    pmcb;
  P_TCB    ptcb;
  void    *pool;
  void    *mem;
  uint32_t res;

  if (queue_id == NULL) {
    return osErrorParameter;
  }

  pmcb = *(((void **)queue_id) + 0);
  pool = *(((void **)queue_id) + 1);

  if ((pmcb == NULL) || (pool == NULL)) {
    return osErrorParameter;
  }

  res = rt_free_box(pool, mail);

  if (res != 0U) {
    return osErrorValue;
  }

  if ((pmcb->p_lnk != NULL) && (pmcb->state == 3U)) {
    // Task is waiting to allocate a message
    if (isr != 0U) {
      rt_psq_enq (pmcb, (U32)pool);
      rt_psh_req ();
    } else {
      mem = rt_alloc_box(pool);
      if (mem != NULL) {
        ptcb = rt_get_first((P_XCB)pmcb);
        rt_ret_val(ptcb, (U32)mem);
        rt_rmv_dly(ptcb);
        rt_dispatch(ptcb);
      }
    }
  }

  return osOK;
}


// Mail Queue Management Public API

/// Create and Initialize mail queue
osMailQId osMailCreate (const osMailQDef_t *queue_def, osThreadId thread_id) {
  if (__get_IPSR() != 0U) {
    return NULL;                                // Not allowed in ISR
  }
  if (((__get_CONTROL() & 1U) == 0U) && (os_running == 0U)) {
    // Privileged and not running
    return   svcMailCreate(queue_def, thread_id);
  } else {
    return __svcMailCreate(queue_def, thread_id);
  }
}

/// Allocate a memory block from a mail
void *osMailAlloc (osMailQId queue_id, uint32_t millisec) {
  if (__get_IPSR() != 0U) {                     // in ISR
    return   sysMailAlloc(queue_id, millisec, 1U);
  } else {                                      // in Thread
    return __sysMailAlloc(queue_id, millisec, 0U);
  }
}

/// Allocate a memory block from a mail and set memory block to zero
void *osMailCAlloc (osMailQId queue_id, uint32_t millisec) {
  void *pool;
  void *mem;

  if (__get_IPSR() != 0U) {                     // in ISR
    mem =   sysMailAlloc(queue_id, millisec, 1U);
  } else {                                      // in Thread
    mem = __sysMailAlloc(queue_id, millisec, 0U);
  }

  pool = *(((void **)queue_id) + 1);

  rt_clr_box(pool, mem);

  return mem;
}

/// Free a memory block from a mail
osStatus osMailFree (osMailQId queue_id, void *mail) {
  if (__get_IPSR() != 0U) {                     // in ISR
    return   sysMailFree(queue_id, mail, 1U);
  } else {                                      // in Thread
    return __sysMailFree(queue_id, mail, 0U);
  }
}

/// Put a mail to a queue
osStatus osMailPut (osMailQId queue_id, void *mail) {
  if (queue_id == NULL) {
    return osErrorParameter;
  }
  if (mail == NULL) {
    return osErrorValue;
  }
  return osMessagePut(*((void **)queue_id), (uint32_t)mail, 0U);
}

/// Get a mail from a queue
os_InRegs osEvent osMailGet (osMailQId queue_id, uint32_t millisec) {
  osEvent ret;

  if (queue_id == NULL) {
    ret.status = osErrorParameter;
    return ret;
  }

  ret = osMessageGet(*((void **)queue_id), millisec);
  if (ret.status == osEventMessage) ret.status = osEventMail;

  return ret;
}


//  ==== RTX Extensions ====

// Service Calls declarations
SVC_0_1(rt_suspend, uint32_t, RET_uint32_t)
SVC_1_0(rt_resume,  void,     uint32_t)


// Public API

/// Suspends the OS task scheduler
uint32_t os_suspend (void) {
  return __rt_suspend();
}

/// Resumes the OS task scheduler
void os_resume (uint32_t sleep_time) {
  __rt_resume(sleep_time);
}
