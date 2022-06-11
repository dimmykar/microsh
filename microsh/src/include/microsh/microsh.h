/**
 * \file            microsh.h
 * \brief           microSH library
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
 * Version:         1.2.0
 */

#ifndef MICROSH_HDR_H
#define MICROSH_HDR_H

#include <stdint.h>
#include <stddef.h>
#include "microsh_config.h"
#include "microrl.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \defgroup        MICROSH Shell for Embedded Systems library
 * \brief           Shell for embedded systems
 * \{
 */

/**
 * \brief           Unused variable macro
 * \param[in]       x: Unused variable
 */
#define MICROSH_UNUSED(x)           ((void)(x))

/**
 * \brief           Calculate size of statically allocated array
 * \param[in]       x: Input array
 * \return          Number of array elements
 */
#define MICROSH_ARRAYSIZE(x)        (sizeof(x) / sizeof((x)[0]))

/**
 * \brief           MicroSH result enumeration
 */
typedef enum {
    microshOK     = 0x00,                        /*!< Everything OK */
    microshERR    = 0x01,                        /*!< Common error */
    microshERRPAR = 0x02,                        /*!< Parameter error */
    microshERRMEM = 0x03,                        /*!< Memory error */
} microshr_t;

typedef enum {
    microshEXEC_OK             = 0x00,          /*!< Successuful command execute */
    microshEXEC_NO_CMD         = 0x01,          /*!< Execute empty command */

    microshEXEC_ERROR          = 0x10,          /*!< Command execute generic error */
    microshEXEC_ERROR_UNK_CMD  = 0x11,          /*!< Unknown command */
    microshEXEC_ERROR_MAX_ARGS = 0x12,          /*!< To many arguments in command */
} microsh_execr_t;

/* Forward declarations */
struct microsh;

/**
 * \brief           Command execute function prototype
 * \param[in]       msh: microSH instance
 * \param[in]       argc: Number of arguments
 * \param[in]       argv: Pointer to arguments
 * \return          \ref microshEXEC_OK on success, member of
 *                      \ref microsh_execr_t enumeration otherwise
 */
typedef int      (*microsh_cmd_fn)(struct microsh* msh, int argc, const char* const *argv);

/**
 * \brief           Optional successful log in callback
 * \param[in]       msh: microSH instance
 */
typedef void     (*microsh_logged_in_fn)(struct microsh* msh);

/**
 * \brief           Shell command structure
 */
typedef struct {
    const char* name;                           /*!< Command name to search for match */
    size_t arg_num;                             /*!< Maximum number of arguments */
    const char* desc;                           /*!< Command description for help */
    microsh_cmd_fn cmd_fn;                      /*!< Command execute function to call */
} microsh_cmd_t;

#if MICROSH_CFG_CONSOLE_SESSIONS
/**
 * \brief           Console session credentials
 */
typedef struct {
    uint32_t login_type;                         /*!< Type of user-defined console session. `0` is used as LOGGED_OUT type in library! */
    char* username;                              /*!< Username of login type */
    char* password;                              /*!< Password of login type */
} microsh_credentials_t;

/**
 * \brief           Сurrent console session status
 */
typedef struct {
    uint32_t login_type;                         /*!< Type of user-defined console session. `0` is used as LOGGED_OUT type in library! */
    size_t attempt;                              /*!< Password enter attempts counter */
    struct flags {
        uint8_t logged_in    : 1;                /*!< User logged into session */
        uint8_t passw_wait   : 1;                /*!< User exists and is allowed to enter password */
        uint8_t reserved     : 6;
    } flags;
} microsh_session_status_t;

/**
 * \brief           Console session context
 */
typedef struct {
    microsh_credentials_t credentials[MICROSH_CFG_MAX_CREDENTIALS];/*!< All available sessions credentials */
    microsh_session_status_t status;             /*!< Сurrent console session status */
    microsh_logged_in_fn logged_in_fn;           /*!< Successful log in callback */
} microsh_session_t;
#endif /* MICROSH_CFG_CONSOLE_SESSIONS */

/**
 * \brief           MicroSH instance
 */
typedef struct microsh {
    microrl_t         mrl;                       /*!< MicroRL context instance */
    microsh_cmd_t     cmds[MICROSH_CFG_NUM_OF_CMDS]; /*!< Array of all registered commands */
    size_t            cmds_index;                /*!< Registered command index counter */
#if MICROSH_CFG_CONSOLE_SESSIONS
    microsh_session_t session;                   /*!< Console session context instance */
#endif /* MICROSH_CFG_CONSOLE_SESSIONS */
} microsh_t;

microshr_t     microsh_init(microsh_t* msh, microrl_output_fn out_fn);

microshr_t     microsh_cmd_register(microsh_t* msh, size_t arg_num, const char* cmd_name,
                                        microsh_cmd_fn cmd_fn, const char* desc);
microshr_t     microsh_cmd_unregister_all(microsh_t* msh);
microsh_cmd_t* microsh_cmd_find(microsh_t* msh, const char* cmd_name);

#if MICROSH_CFG_CONSOLE_SESSIONS
microshr_t     microsh_session_init(microsh_t* msh, const microsh_credentials_t* cred, size_t cred_num,
                                        microsh_logged_in_fn logged_in_cb);
uint8_t        microsh_session_is_logged_in(microsh_t* msh);
uint32_t       microsh_session_get_login_type(microsh_t* msh);
microshr_t     microsh_session_logout(microsh_t* msh);
#endif /* MICROSH_CFG_CONSOLE_SESSIONS */

/**
 * \}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MICROSH_HDR_H */
