with import <nixpkgs> {};
pkgs.mkShell {
	name = "application"; # change this to something more catchy

	NIX_LD_LIBRARY_PATH = lib.makeLibraryPath [
		# $(nix path-info nixpkgs#libuv)/lib
		libuv
		libudev-zero
	];

	NIX_LD = lib.fileContents "${stdenv.cc}/nix-support/dynamic-linker";

	packages = with pkgs; [
		arduino-cli
		cmake
		libgcc
	];

	shellHook = ''
		export LD_LIBRARY_PATH=$NIX_LD_LIBRARY_PATH
	'';
}
