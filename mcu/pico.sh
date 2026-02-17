port="/dev/ttyACM0"

project_path="/home/huy/repositories/led_strip/mcu"

build_cmd="cmake -B ${project_path}/build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -G Ninja"
compile_cmd="ninja -C ${project_path}/build"


sudo chmod o+rw $port

cd ${project_path}

build() {
	echo
	echo '----------------Building----------------'
	echo
	
	if [[ ${IN_NIX_SHELL} == "impure" ]]; then
		nix develop --command ${build_cmd}
	else
		build_cmd
	fi
}

compile() {
	echo
	echo '----------------Compiling----------------'
	echo
	
	if [[ ${IN_NIX_SHELL} == "impure" ]]; then
		nix develop --command ${compile_cmd}
	else
		compile_cmd
	fi
}

upload() {
	echo
	echo '----------------Uploading----------------'
	echo
	sudo picotool load -f ${project_path}/build/pico2w_minimal.uf2
	$upload_cmd
	echo
}

reboot() {
	echo
	echo '----------------Rebooting----------------'
	echo
	sudo chmod o+rw $port
	if [[ $? == 0 ]]; then
		exit 1
	fi
	sudo picotool reboot -f
}

monitor() {
	echo
	echo '----------------Start Monitor----------------'
	echo
	sudo chmod o+rw $port
	if [[ $? == 0 ]]; then
		exit 1
	fi
	arduino-cli monitor -p /dev/ttyACM0 --config 115200;
}

bcum() {
	build
	if [[ $? == 0 ]]; then
		exit 1
	fi
	cum
}

cum() {
	cu
	if [[ $? == 0 ]]; then
		exit 1
	fi
	monitor
}

cu() {
	compile
	if [[ $? == 0 ]]; then
		exit 1
	fi
	upload
}


if [ $# -eq 0 ]; then
	bcum
	exit 0
elif declare -f "$1" > /dev/null; then
	if [[ $2 == '-r' ]]; then
		echo ========================= removing build directory =========================
		rm -rf ${project_path}/build
	fi

	"$@"
	exit 0
else
	echo "'$1' is not a known function name" >&2
	exit 1
fi
