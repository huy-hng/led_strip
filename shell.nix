with import <nixpkgs> {};
pkgs.mkShell {
	name = "application"; # change this to something more catchy

	# Here is where you will add all the libraries required by your native modules
	# You can use the following one-liner to find out which ones you need.
	# Just make sure you have `gcc` installed.
	# `find .venv/ -type f -name "*.so" | xargs ldd | grep "not found" | sort | uniq`
	NIX_LD_LIBRARY_PATH = lib.makeLibraryPath [
		stdenv.cc.cc # libstdc++
		zlib # libz (for numpy)

		glib
		libGL
		fontconfig
		xorg.libX11
		libxkbcommon
		freetype
		dbus

		# libxcb
		# libxcb-wm
		# libxcb-util
		# libxcb-image
		# libxcb-errors
		# libxcb-cursor
		# libxcb-keysyms
		# libxcb-render-util

	];

	NIX_LD = lib.fileContents "${stdenv.cc}/nix-support/dynamic-linker";

	packages = with pkgs; [
		# If you're using Poetry, comment out `uv` and uncomment the following two lines:
		#   python313 # set to your Python version
		#   poetry
		# uv
		pipenv
	];

	# Uncomment if you're using Poetry - since we're using a Nix-provided `python`
	# as opposed to an unpatched one, we need to explicitly inform it of the
	# dynamic library path.
	#
	shellHook = ''
		export LD_LIBRARY_PATH=$NIX_LD_LIBRARY_PATH
	'';
	
	
	# QT_QPA_PLATFORM_PLUGIN_PATH="/home/huy/.local/share/virtualenvs/led_strip-lAiA01QS/lib/python3.11/site-packages/PyQt5/Qt5/plugins/platforms";
	QT_QPA_PLATFORM_PLUGIN_PATH="${qt5.qtbase.bin}/lib/qt-${qt5.qtbase.version}/plugins/platforms";
}
