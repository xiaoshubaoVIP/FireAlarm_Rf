#include "event.h"
#include "list.h"

typedef struct 
{
    FLAG f;
} event_t;



void Event_Init(void)
{

}

void Event_Handler(void)
{

}

void Event_Registere()
{
    
}

void Event_Set(p_event event, FLAG flag)
{

}


// #ifndef NULL
// #define NULL ((void*)0)
// #endif

// static void (*function[SEQ_NUM])(void);
// volatile static uint32_t sequencer;

// /**
//  * @brief ����������
//  */
// void Sequencer_Handler(void)
// {
//     for (uint8_t i = 0; sequencer > 0; i++) {
//         if (i == SEQ_NUM)
//             i = 0;

//         if ((sequencer & (1 << i)) > 0) {
//             sequencer &= ~(1 << i);
//             if (function[i] != NULL)
//                 function[i]();
//         }
//     }
// }

// /**
//  * @brief ������ע������
//  * 
//  * @param seq ������id
//  * @param callback ������
//  */
// void Sequencer_Register(enum sequencer seq, void (*callback)(void))
// {
//     function[seq] = callback;
// }

// /**
//  * @brief ��������
//  * 
//  * @param seq ��������
//  */
// void Sequencer_Set(enum sequencer seq)
// {
//     sequencer |= (1 << seq);
// }

