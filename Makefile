# mac
build:
	clang++ main.cpp ACO.cpp -o main
	./main 1 > output_case1.txt
	./main 2 > output_case2.txt
	./main 3 > output_case3.txt

# windows
# build:
# 	g++ main.cpp ACO.cpp -o main
# 	main.exe 1 > output_case1.txt
# 	main.exe 2 > output_case2.txt
# 	main.exe 3 > output_case3.txt

