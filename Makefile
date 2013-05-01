
FLAGS := -D GL_GLEXT_PROTOTYPES 2>log
LIBS := -lGL -lglfw

MarchingCubes : main.o helperfuns.o math.o
	g++ $^ -o $@ $(FLAGS) $(LIBS)

main.o : main.cc HelperFuns.h
	g++ -c $< -o $@ $(FLAGS)

helperfuns.o : HelperFuns.cc HelperFuns.h
	g++ -c $< -o $@ $(FLAGS)

math.o : Math.cc Math.h
	g++ -c $< -o $@ $(FLAGS)

.PHONY : clean
clean :
	rm -f main.o helperfuns.o math.o MarchingCubes