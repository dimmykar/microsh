/**
 * \file            stm32_misc.c
 * \brief           STM32 platform specific implementation routines (for STM32F4, rewrite for your MCU)
 */

/*
 * Copyright (c) 2022 Dmitry KARASEV
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This file is part of microSH - Shell for Embedded Systems library.
 *
 * Author:          Dmitry KARASEV <karasevsdmitry@yandex.ru>
 * Version:         1.1.0-dev
 */

#include <string.h>
#include <stdlib.h>
#include "stm32f4xx.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_usart.h"
#include "microsh.h"

#define USART_TX_Pin                LL_GPIO_PIN_2
#define USART_RX_Pin                LL_GPIO_PIN_3
#define USART_GPIO_Port             GPIOA
#define USART_GPIO_AF_NUM           LL_GPIO_AF_7
#define USART_GPIO_ENABLE_CLOCK()   LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA)
#define USART_UART_ENABLE_CLOCK()   LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2)
#define USART_PERIFH                USART2

#define _STM32_DEMO_VER             "1.0"

#define _ENDLINE_SEQ                MICRORL_CFG_END_LINE

/* Definition commands word */
#define _CMD_HELP                   "help"
#define _CMD_CLEAR                  "clear"
#define _CMD_SERNUM                 "sernum"
#define _CMD_LOGOUT                 "logout"

/* Arguments for set/clear */
#define _SCMD_RD                    "?"
#define _SCMD_SAVE                  "save"

#define _NUM_OF_CMD                 4
#define _NUM_OF_SETCLEAR_SCMD       2

/* Available  commands */
char* keyword[] = {_CMD_HELP, _CMD_CLEAR, _CMD_SERNUM, _CMD_LOGOUT};

/* 'read/save' command argements */
char* read_save_key[] = {_SCMD_RD, _SCMD_SAVE};

/* Array for comletion */
char* compl_word[_NUM_OF_CMD + 1];

/* Variable changeable with commands */
uint32_t device_sn = 0;

static int help_cmd(microsh_t* msh, int argc, const char* const *argv);
static int clear_screen_cmd(microsh_t* msh, int argc, const char* const *argv);
static int sernum_cmd(microsh_t* msh, int argc, const char* const *argv);
#if MICROSH_CFG_CONSOLE_SESSIONS
static int logout_cmd(microsh_t* msh, int argc, const char* const *argv);
#endif /* MICROSH_CFG_CONSOLE_SESSIONS */

/**
 * \brief           Init STM32F4 platform
 */
void init(void) {
    LL_USART_InitTypeDef USART_InitStruct = {0};
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
  
    USART_GPIO_ENABLE_CLOCK();
    USART_UART_ENABLE_CLOCK();
    __DSB();

    GPIO_InitStruct.Pin = USART_TX_Pin | USART_RX_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    GPIO_InitStruct.Alternate = USART_GPIO_AF_NUM;
    LL_GPIO_Init(USART_GPIO_Port, &GPIO_InitStruct);

    USART_InitStruct.BaudRate = 115200;
    USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
    USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
    USART_InitStruct.Parity = LL_USART_PARITY_NONE;
    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
    LL_USART_Init(USART_PERIFH, &USART_InitStruct);

    LL_USART_DisableIT_CTS(USART_PERIFH);
    LL_USART_ConfigAsyncMode(USART_PERIFH);

    LL_USART_Enable(USART_PERIFH);
}
#if MICROSH_CFG_CONSOLE_SESSIONS
/**
 * \brief           Register commands that may be used in authorization process
 * \param[in]       msh: \ref microsh_t working instance
 * \return          \ref microshOK on success, member of \ref microshr_t otherwise
 */
microshr_t register_auth_commands(microsh_t* msh) {
    microshr_t result = microshOK;

    result |= microsh_cmd_register(msh, 1, _CMD_HELP,   help_cmd,         NULL);

    return result;
}
#endif /* MICROSH_CFG_CONSOLE_SESSIONS */

/**
 * \brief           Register all commands used by shell
 * \param[in]       msh: \ref microsh_t working instance
 * \return          \ref microshOK on success, member of \ref microshr_t otherwise
 */
microshr_t register_all_commands(microsh_t* msh) {
    microshr_t result = microshOK;

    result |= microsh_cmd_register(msh, 1, _CMD_HELP,   help_cmd,         NULL);
    result |= microsh_cmd_register(msh, 1, _CMD_CLEAR,  clear_screen_cmd, NULL);
    result |= microsh_cmd_register(msh, 2, _CMD_SERNUM, sernum_cmd,       NULL);
#if MICROSH_CFG_CONSOLE_SESSIONS
    result |= microsh_cmd_register(msh, 1, _CMD_LOGOUT, logout_cmd,       NULL);
#endif /* MICROSH_CFG_CONSOLE_SESSIONS */

    return result;
}

/**
 * \brief           Print string to IO stream
 * \param[in]       str: Output string
 * \return          The number of characters that would have been written,
 *                      not counting the terminating null character.
 */
static int print(const char* str) {
    uint32_t i = 0;

    while (str[i] != 0) {
        while (!LL_USART_IsActiveFlag_TXE(USART_PERIFH)) {}
        LL_USART_TransmitData8(USART_PERIFH, str[i++]);
    }

    return i;
}

/**
 * \brief           Print to IO stream callback for MicroRL library
 * \param[in]       mrl: \ref microrl_t working instance
 * \param[in]       str: Output string
 * \return          The number of characters that would have been written,
 *                      not counting the terminating null character.
 */
int microrl_print(microrl_t* mrl, const char* str) {
    MICROSH_UNUSED(mrl);
    return print(str);
}

/**
 * \brief           Get char user pressed
 * \return          Input character
 */
char get_char(void) {
    while (!LL_USART_IsActiveFlag_RXNE(USART_PERIFH)) {}
    return (char)LL_USART_ReceiveData8(USART_PERIFH);
}

/**
 * \brief           Makes `unsigned 32-bit` value from ascii char array
 * \param[in]       str: Input string with value to convert
 * \param[out]      val: `unsigned 32-bit` data to be converted
 */
static void str_to_u32(char* str, uint32_t* val) {
    uint32_t temp = 0;
 
    for (uint8_t i = 0; str[i] >= 0x30 && str[i] <= 0x39; ++i) {
        temp = temp + (str[i] & 0x0F);
        temp = temp * 10;
    }
    temp = temp / 10;

    *val = temp;
}

/**
 * \brief           Makes ascii char array from `unsigned 32-bit` value
 * \param[in]       val: `unsigned 32-bit` data to be converted
 * \param[out]      str: Minimum `11-bytes` long array to write value to
 */
static void u32_to_str(uint32_t* val, char* str) {
    uint32_t v = *val;
    size_t s = 0;
    char t;

    size_t n;
    for (n = 0; v > 0; v /= 10) {
        str[s + n++] = "0123456789"[v % 10];
    }

    /* Reverse a string */
    for (size_t i = 0; i < n / 2; ++i) {
        t = str[s + i];
        str[s + i] = str[s + n - i - 1];
        str[s + n - i - 1] = t;
    }

    if (val == NULL) {
        str[n++] = '0';  /* Handle special case */
    }
}

/**
 * \brief           SERNUM ? command callback
 */
static void read_sernum(void) {
    char sn_str[11] = {0};
    uint32_t sn = device_sn;
    u32_to_str(&sn, sn_str);

    print("\tS/N ");
    print(sn_str);
    print(_ENDLINE_SEQ);
}

/**
 * \brief           SERNUM VALUE command callback
 * \param[in]       str_val: New serial number value
 */
static void set_sernum(char* str_val) {
    uint32_t sn = 0;

    str_to_u32(str_val, &sn);
    if (sn != 0) {
        device_sn = sn;
        
        print("\tset S/N ");
        print(str_val);
        print(_ENDLINE_SEQ);
        return;
    }

    print("\tS/N not set"_ENDLINE_SEQ);
}

/**
 * \brief           SERNUM SAVE command callback
 */
static void save_sernum(void) {
    /* To simplify the code, no implementation of writing SN to FLASH OTP memory is provided here */
    print("\tS/N save done"_ENDLINE_SEQ);
}

/**
 * \brief           HELP command execution
 * \param[in]       msh: \ref microsh_t working instance
 * \param[in]       argc: argument count
 * \param[in]       argv: pointer array to token string
 * \return          \ref microshEXEC_OK on success, member of
 *                      \ref microsh_execr_t enumeration otherwise
 */
int help_cmd(microsh_t* msh, int argc, const char* const *argv) {
    MICRORL_UNUSED(msh);

    print("MicroSH library DEMO v");
    print(_STM32_DEMO_VER);
    print(_ENDLINE_SEQ);

    print("Use TAB key for completion"_ENDLINE_SEQ);
#if MICROSH_CFG_CONSOLE_SESSIONS
    if (!msh->session.status.flags.logged_in) {
        print(_ENDLINE_SEQ"You must log in to one of the sessions."_ENDLINE_SEQ);
        print("After authorization, session commands will be available."_ENDLINE_SEQ);
        print("Different commands may be available for different sessions."_ENDLINE_SEQ);
    } else {
#endif /* MICROSH_CFG_CONSOLE_SESSIONS */
        print("List of commands:"_ENDLINE_SEQ);
        print("\tclear               - clear screen"_ENDLINE_SEQ);
        print("\tsernum ?            - read serial number value"_ENDLINE_SEQ);
        print("\tsernum VALUE        - set serial number value"_ENDLINE_SEQ);
        print("\tsernum save         - save serial number value to flash"_ENDLINE_SEQ);
        print("\tlogout              - end an authorized session"_ENDLINE_SEQ);
#if MICROSH_CFG_CONSOLE_SESSIONS
    }
#endif /* MICROSH_CFG_CONSOLE_SESSIONS */

    return microshEXEC_OK;
}

/**
 * \brief           CLEAR command execution
 * \param[in]       msh: \ref microsh_t working instance
 * \param[in]       argc: argument count
 * \param[in]       argv: pointer array to token string
 * \return          \ref microshEXEC_OK on success, member of
 *                      \ref microsh_execr_t enumeration otherwise
 */
int clear_screen_cmd(microsh_t* msh, int argc, const char* const *argv) {
    MICRORL_UNUSED(msh);

    print("\033[2J");    /* ESC seq for clear entire screen */
    print("\033[H");     /* ESC seq for move cursor at left-top corner */

    return microshEXEC_OK;
}

/**
 * \brief           SERNUM command execution
 * \param[in]       msh: \ref microsh_t working instance
 * \param[in]       argc: argument count
 * \param[in]       argv: pointer array to token string
 * \return          \ref microshEXEC_OK on success, member of
 *                      \ref microsh_execr_t enumeration otherwise
 */
int sernum_cmd(microsh_t* msh, int argc, const char* const *argv) {
    MICRORL_UNUSED(msh);

    size_t i = 0;

    if (++i < argc) {
        if (strcmp(argv[i], _SCMD_RD) == 0) {
            read_sernum();
        } else if (strcmp(argv[i], _SCMD_SAVE) == 0) {
            save_sernum();
        } else {
            set_sernum((char*)argv[i]);
        }
    } else {
        print("Read or specify serial number"_ENDLINE_SEQ);
        return microshEXEC_ERROR;
    }

    return microshEXEC_OK;
}

#if MICROSH_CFG_CONSOLE_SESSIONS
/**
 * \brief           LOGOUT command execution
 * \param[in]       msh: \ref microsh_t working instance
 * \param[in]       argc: argument count
 * \param[in]       argv: pointer array to token string
 * \return          \ref microshEXEC_OK on success, member of
 *                      \ref microsh_execr_t enumeration otherwise
 */
int logout_cmd(microsh_t* msh, int argc, const char* const *argv) {
    microsh_session_logout(msh);
    microsh_cmd_unregister_all(msh);
    print("Logged out"_ENDLINE_SEQ);

    return microshEXEC_OK;
}
#endif /* MICROSH_CFG_CONSOLE_SESSIONS */

#if MICRORL_CFG_USE_COMPLETE || __DOXYGEN__
/**
 * \brief           Completion callback for MicroRL library
 * \param[in,out]   mrl: \ref microrl_t working instance
 * \param[in]       argc: argument count
 * \param[in]       argv: pointer array to token string
 * \return          NULL-terminated string, contain complite variant split by 'Whitespace'
 */
char** complet(microrl_t* mrl, int argc, const char* const *argv) {
    MICRORL_UNUSED(mrl);
    int j = 0;

    compl_word[0] = NULL;

    /* If there is token in cmdline */
    if (argc == 1) {
        /* Get last entered token */
        char* bit = (char*)argv[argc - 1];
        /* Iterate through our available token and match it */
        for (int i = 0; i < _NUM_OF_CMD; ++i) {
            /* If token is matched (text is part of our token starting from 0 char) */
            if (strstr(keyword[i], bit) == keyword[i]) {
                /* Add it to completion set */
                compl_word[j++] = keyword[i];
            }
        }
    }  else if ((argc > 1) && (strcmp(argv[0], _CMD_SERNUM) == 0)) {   /* If command needs subcommands */
        /* Iterate through subcommand */
        for (int i = 0; i < _NUM_OF_SETCLEAR_SCMD; ++i) {
            if (strstr(read_save_key[i], argv[argc - 1]) == read_save_key[i]) {
                compl_word[j++] = read_save_key[i];
            }
        }
    } else {    /* If there is no token in cmdline, just print all available token */
        for (; j < _NUM_OF_CMD; ++j) {
            compl_word[j] = keyword[j];
        }
    }

    /* Note! Last ptr in array always must be NULL!!! */
    compl_word[j] = NULL;

    /* Return set of variants */
    return compl_word;
}
#endif /* MICRORL_CFG_USE_COMPLETE || __DOXYGEN__ */

#if MICRORL_CFG_USE_CTRL_C || __DOXYGEN__
/**
 * \brief           Ctrl+C terminal signal function
 * \param[in]       mrl: \ref microrl_t working instance
 */
void sigint(microrl_t* mrl) {
    microrl_print(mrl, "^C is caught!"_ENDLINE_SEQ);
}
#endif /* MICRORL_CFG_USE_CTRL_C || __DOXYGEN__ */
