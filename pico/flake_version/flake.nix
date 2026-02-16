{
	description = "Minimal Raspberry Pi Pico 2 W (RP2350) flake environment";

	inputs = {
		nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
	};

	outputs = { self, nixpkgs }:
		let
			system = "x86_64-linux";
			pkgs = nixpkgs.legacyPackages.${system};
			picoSdk = pkgs.fetchFromGitHub {
				owner = "raspberrypi";
				repo = "pico-sdk";
				rev = "master";  # pin a release tag for stability if needed
				sha256 = "0000000000000000000000000000000000000000000000000000"; # replace with nix-prefetch-git
			};
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
					pkgs.arm-none-eabi-gcc
					pkgs.openocd
				];

				shellHook = ''
					export PICO_SDK_PATH=${picoSdk}
					export PATH=$PATH:${pkgs.arm-none-eabi-gcc}/bin
					echo "Pico 2 W SDK flake environment loaded!"
					'';
				};
		};
}

