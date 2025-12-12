All: Debug

configure:
	cmake -B build -S . -G "Ninja Multi-Config"

debug:
	cmake --build build --config Debug

release:
	cmake --build build --config Release

clean:
	rm -rf build/*

format:
	clang-format -i src/** || clang-format -i src/**/** || clang-format -i src/**/**/** || clang-format -i src/**/**/**/**

.PHONY: All configure debug release clean format
