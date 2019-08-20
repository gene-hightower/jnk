flags += -Wall -Og -ggdb3 -D_POSIX_C_SOURCE=200112L

flags += -fsanitize=address -fsanitize=undefined -fsanitize-address-use-after-scope

CXXFLAGS += -std=c++17 $(flags)
CFLAGS   += -std=c17 $(flags)


LDLIBS += -lbsd
