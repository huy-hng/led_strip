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

		xorg.libX11 # for matplotlib

		# glib
		# libGL
		# fontconfig
		# libxkbcommon
		# freetype
		# dbus
	];

	NIX_LD = lib.fileContents "${stdenv.cc}/nix-support/dynamic-linker";

	packages = with pkgs; [
		# If you're using Poetry, comment out `uv` and uncomment the following two lines:
		#   python313 # set to your Python version
		#   poetry
		pipenv
		uv
		python314Packages.pyqt6
	];

	# Uncomment if you're using Poetry - since we're using a Nix-provided `python`
	# as opposed to an unpatched one, we need to explicitly inform it of the
	# dynamic library path.
	#
	shellHook = ''
		export LD_LIBRARY_PATH=$NIX_LD_LIBRARY_PATH
	'';
	
	
	# QT_QPA_PLATFORM_PLUGIN_PATH="/home/huy/.local/share/virtualenvs/led_strip-lAiA01QS/lib/python3.11/site-packages/PyQt6/Qt6/plugins/platforms";
	# /home/huy/.local/share/virtualenvs/led_strip-lAiA01QS/lib/python3.11/site-packages/PyQt6
	# QT_QPA_PLATFORM_PLUGIN_PATH="${qt5.qtbase.bin}/lib/qt-${qt5.qtbase.version}/plugins/platforms";
	# QT_QPA_PLATFORM_PLUGIN_PATH="${qt6.qtbase.bin}/lib/qt-${qt6.qtbase.version}/plugins/platforms";
}
