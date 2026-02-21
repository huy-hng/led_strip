{
	description = "Raspberry Pi Pico 2 W (RP2350) flake environment";

	inputs = {
		nixpkgs.url = "github:NixOS/nixpkgs/25.11";
	};

	outputs = { self, nixpkgs }:
		let
			system = "x86_64-linux";
			pkgs = nixpkgs.legacyPackages.${system};
			# PICO_TOOLCHAIN_PREFIX = "arm-none-eabi-";

			picoSdk = builtins.fetchGit {
				url = "https://github.com/raspberrypi/pico-sdk.git";
				rev = "a1438dff1d38bd9c65dbd693f0e5db4b9ae91779";  # version 2.2.0
				submodules = true;
			};

			python_inputs = [
				pkgs.git
				pkgs.pkg-config
				pkgs.python3
				pkgs.python3Packages.pip
				pkgs.unzip
				pkgs.openocd

			];

			pico_inputs = [
				pkgs.gcc-arm-embedded
				pkgs.cmake
				pkgs.ninja
				pkgs.picotool
				# (pico-sdk.override { withSubmodules = true; })

				pkgs.clang
				pkgs.clang-tools
			];

		in {

			devShells.${system}.default = pkgs.mkShell {
				buildInputs = pico_inputs;

				PICO_SDK_PATH = picoSdk;
				
				PATH = "$PATH:${pkgs.gcc-arm-embedded}/bin";

				# CC = "${PICO_TOOLCHAIN_PREFIX}gcc";
				# CXX = "${PICO_TOOLCHAIN_PREFIX}g++";
				# AS = "${PICO_TOOLCHAIN_PREFIX}gcc";
				# AR = "${PICO_TOOLCHAIN_PREFIX}ar";
				# LD = "${PICO_TOOLCHAIN_PREFIX}ld";
				# NM = "${PICO_TOOLCHAIN_PREFIX}nm";
				# OBJCOPY = "${PICO_TOOLCHAIN_PREFIX}objcopy";
				# OBJDUMP = "${PICO_TOOLCHAIN_PREFIX}objdump";
				# STRIP = "${PICO_TOOLCHAIN_PREFIX}strip";

				shellHook = ''
					zsh
				'';


				buildPhase = ''
					echo $PICO_SDK_PATH

					# mkdir -p $out
					# cmake . -DPICO_BOARD=pico2_w -DPICO_PLATFORM=rp2350 -DCMAKE_BUILD_TYPE=Release -G Ninja
					# cd build; cmake ..   -DPICO_BOARD=pico2_w   -DPICO_PLATFORM=rp2350   -DCMAKE_BUILD_TYPE=Release -G Ninja
					echo $out
					'';

			};

			# packages.${system}.default = pkgs.stdenv.mkDerivation {
			# 	name = "app";
			# 	version = "1.0";
			# 	src = ./.;

			# 	# PATH=$PATH:${pkgs.gcc-arm-embedded}/bin
			# 	buildInputs = commonInputs;

			# 	CC = pkgs.gcc;
			# 	# CC = PICO_TOOLCHAIN_PREFIX + "gcc";
			# 	# CXX = PICO_TOOLCHAIN_PREFIX + "g++";
			# 	# AS = PICO_TOOLCHAIN_PREFIX + "gcc";

			# 	# PICO_SDK_PATH = picoSdk;
			# 	PICO_SDK_PATH = pkgs.pico-sdk + "/lib/pico-sdk";
			# 	configurePhase = '''';

			# 	buildPhase = ''
			# 		export CC=${PICO_TOOLCHAIN_PREFIX}gcc
			# 		echo $PICO_SDK_PATH

			# 		# mkdir -p $out
			# 		# cmake . -DPICO_BOARD=pico2_w -DPICO_PLATFORM=rp2350 -DCMAKE_BUILD_TYPE=Release -G Ninja
			# 		# cd build; cmake ..   -DPICO_BOARD=pico2_w   -DPICO_PLATFORM=rp2350   -DCMAKE_BUILD_TYPE=Release -G Ninja
			# 		echo $out
			# 	'';

			# 	installPhase = ''
			# 		mkdir -p $out/bin
			# 		cp app $out/bin/
			# 	'';

			# };
		};
}

