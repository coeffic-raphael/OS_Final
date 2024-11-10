# Compiler variables and flags
CXX = g++  # The C++ compiler to use
CXXFLAGS = -std=c++17 -pthread -Wall -Wextra -g -O0 -fprofile-arcs -ftest-coverage  # Compiler flags for debugging and code coverage

# List of source files
SRC = main.cpp Server.cpp CommandHandler.cpp ActiveObject.cpp ThreadPool.cpp

# List of object files for normal compilation
OBJS = main.o Server.o CommandHandler.o ActiveObject.o ThreadPool.o

# Name of the executable
EXEC = server

# Main target - builds the executable
all: $(EXEC)

# Linking rule for the normal executable
$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(OBJS) -lgcov  # Link the object files into an executable, including gcov for code coverage

# Compilation rules for normal compilation (%.o means any .cpp file)
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@  # Compile each .cpp file into a corresponding .o file

# Target to run the server
run: $(EXEC)
	./$(EXEC)  # Execute the server

# Generate code coverage report
coverage: $(EXEC)
	./$(EXEC)  # Run the executable to collect coverage data
	@gcov $(SRC) > coverage.txt  # Run gcov on the source files and output the coverage report to coverage.txt

# Clean target - removes object files, executables, and coverage data
clean:
	rm -f *.o $(EXEC) *.gcda *.gcno *.gcov coverage.txt valgrind_memcheck.txt valgrind_helgrind.txt callgrind.out callgrind_summary.txt # Remove all object files, the executable, and any coverage data files

# Valgrind memory check target
memcheck: $(EXEC)
	valgrind --leak-check=full --show-leak-kinds=all ./$(EXEC) 2>&1 | tee valgrind_memcheck.txt  # Run Valgrind to check for memory leaks

# Valgrind thread analysis (Helgrind)
helgrind: $(EXEC)
	valgrind --tool=helgrind ./$(EXEC) 2>&1 | tee valgrind_helgrind.txt  # Run Valgrind's Helgrind tool to check for race conditions
	
c: $(EXEC)
	valgrind --tool=callgrind ./$(EXEC)
# Valgrind profiling target (Callgrind)
callgrind: $(EXEC)
	valgrind --tool=callgrind --callgrind-out-file=callgrind.out ./$(EXEC)
	callgrind_annotate callgrind.out --inclusive=yes --tree=both > callgrind_summary.txt  # Generate a filtered summary

# KCachegrind profiling visualization
kcachegrind: callgrind
	kcachegrind callgrind.out  # Use KCachegrind to visualize Callgrind output
