# Changelog


## Develop

1.  Update `microrl-remaser` command line library to `v2.2.3`
2.  Fix all possible warnings at compilation


## v1.1.1

1.  Fix possible commands array overflow when registering too many commands

## v1.1.0

1.  Change names for command-related APIs
    - There is a mistake in the accepted naming of command-related APIs. This breaks backwards compatibility, making it wrong to make these changes in a minor update. However, due to the unpopularity of the library, one can take such liberties
2.  Add new useful APIs
    - Add command find API `microsh_cmd_find()`. Using this API you can find by command name the registered command entry
    - Add logged in check API `microsh_session_is_logged_in()`. Using this API you can check current session status: logged in or logged out.
    - Add get current session login type API `microsh_session_get_login_type()`. Using this API you can get user-defined session login type to find out which username you are authorized

## v1.0.0

First release

1.  Add maximum number of arguments for each command with a check for exceeding the maximum number of arguments
2.  Add command result codes
3.  Added minimal post command execute implementation with optional execution error logging
4.  Add deleting of all registered commands API
5.  Update `microrl-remaser` command line library to `v2.2.2`
6.  Implement console sessions for implementing multi-user work of the shell
    - The `login` command is reserved by the library and should not be used by the user
    - After initializing the library, it is necessary to initialize sessions by session credentials
    - Password input is masked with `*` character (echo is disabled)
    - After authorization, for each user can be assigned different commands depending on the privileges.
    - Optionally, some set of commands can be registered that available during authorization process

## v0.1.0

Minimal working implementation
