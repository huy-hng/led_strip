{
	description = "Raspberry Pi Pico 2 W (RP2350) flake environment";

	inputs = {
		nixpkgs.url = "github:NixOS/nixpkgs/25.11";
	};

	outputs = { self, nixpkgs }:
		let
			system = "x86_64-linux";
			pkgs = nixpkgs.legacyPackages.${system};

			picoSdk = pkgs.fetchFromGitHub {
				owner = "raspberrypi";
				repo = "pico-sdk";
				rev = "master";  # pin a release tag for stability if needed
				fetchSubmodules = true;
				# run nix flake prefetch github:raspberrypi/pico-sdk to get the sha
				sha256 = "sha256-hQdEZD84/cnLSzP5Xr9vbOGROQz4BjeVOnvbyhe6rfM="; # replace with nix-prefetch-git
			};

			# picoSdk = builtins.fetchGit {
			# 	url = "https://github.com/raspberrypi/pico-sdk.git";
			# 	rev = "master";  # or a specific tag
			# 	submodules = true;
			# 	hash = "sha256-hQdEZD84/cnLSzP5Xr9vbOGROQz4BjeVOnvbyhe6rfM="; # replace with nix-prefetch-git
			# };

			PICO_TOOLCHAIN_PREFIX = "arm-none-eabi-";
		in {
			devShells.${system}.default = pkgs.mkShell {
				buildInputs = [
					pkgs.git
					pkgs.cmake
					pkgs.ninja
					pkgs.gcc
					pkgs.pkg-config
					pkgs.python3
					pkgs.python3Packages.pip
					pkgs.unzip
					pkgs.gcc-arm-embedded
					pkgs.openocd
				];

				shellHook = ''
					export PICO_SDK_PATH=${picoSdk}
					export PATH=$PATH:${pkgs.gcc-arm-embedded}/bin
					export CC=${PICO_TOOLCHAIN_PREFIX}gcc
					export CXX=${PICO_TOOLCHAIN_PREFIX}g++
					export AS=${PICO_TOOLCHAIN_PREFIX}gcc
					export AR=${PICO_TOOLCHAIN_PREFIX}ar
					export LD=${PICO_TOOLCHAIN_PREFIX}ld
					export NM=${PICO_TOOLCHAIN_PREFIX}nm
					export OBJCOPY=${PICO_TOOLCHAIN_PREFIX}objcopy
					export OBJDUMP=${PICO_TOOLCHAIN_PREFIX}objdump
					export STRIP=${PICO_TOOLCHAIN_PREFIX}strip

					# git submodule update --init ${picoSdk}
					echo "Pico 2 W SDK flake environment loaded!"
				'';
			};
		};
}

