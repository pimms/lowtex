SRC := $(shell find . | egrep "*.(cpp)$$")
HDR := $(shell find . | egrep "*.(h)$$")

daily: $(SRC) $(HDR)
	g++ -std=c++14 $(SRC) -g -o daily
