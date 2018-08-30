#!/bin/bash

astyle                \
--style=linux         \
--indent=spaces=8     \
--pad-oper            \
--pad-comma           \
--pad-header          \
--unpad-paren         \
--delete-empty-lines  \
--lineend=linux       \
--remove-braces       \
--convert-tabs        \
--mode=c              \
--suffix=none         \
--recursive           \
--formatted           \
"*.c" "*.h"
