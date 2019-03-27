CXX = clang++
CXXFLAGS = -Wfatal-errors --std=c++17 -O3 -march=native -pthread
INCLUDE = -I../include

all: ./bin/test002.out ./bin/test003.out ./bin/test004.out ./bin/test005.out

./bin/test001.out: test001.cpp
	$(CXX) $(CXXFLAGS) ${INCLUDE} -o $(@:.out=.x) $<
	$(@:.out=.x) $@

./bin/test002.out: test002.cpp
	$(CXX) $(CXXFLAGS) ${INCLUDE} -o $(@:.out=.x) $<
	$(@:.out=.x) $@ $(@:.out=.err)

./bin/test003.out: test003.cpp
	$(CXX) $(CXXFLAGS) ${INCLUDE} -o $(@:.out=.x) $<
	$(@:.out=.x) $@ $(@:.out=.err)

./bin/test004.out: test004.cpp
	$(CXX) $(CXXFLAGS) ${INCLUDE} -o $(@:.out=.x) $<
	$(@:.out=.x) $@ $(@:.out=.err)

./bin/test005.out: test005.cpp
	$(CXX) $(CXXFLAGS) ${INCLUDE} -o $(@:.out=.x) $<
	$(@:.out=.x) $@ $(@:.out=.err)

clean:
	rm ./bin/*