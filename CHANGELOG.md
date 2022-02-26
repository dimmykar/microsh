# Changelog


## Develop



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
