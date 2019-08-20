CXXFLAGS += -Wall -std=c++17 -Og -ggdb3 -D_POSIX_C_SOURCE=200112L
CFLAGS += -Wall -std=c17 -Og -ggdb3 -D_POSIX_C_SOURCE=200112L

# -fsanitize=address -fsanitize=undefined -fsanitize-address-use-after-scope

LDLIBS += -lbsd
