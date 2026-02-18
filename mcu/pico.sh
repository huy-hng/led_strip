port="/dev/ttyACM0"
project_path="/home/huy/repositories/led_strip/mcu"

print_header() {
	echo
	echo "----------------$1----------------"
	echo
}

build() {
	print_header "Building"

	if $rm_build; then
		echo ======== Removing Build Directory ========
		rm -rf ${project_path}/build
	fi
	
	cmake -B ${project_path}/build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -G Ninja
}

compile() {
	print_header "Compiling"
	ninja -C ${project_path}/build
	# if [[ ${IN_NIX_SHELL} == "impure" ]]; then
	# 	nix develop --command "${compile_cmd}"
}

upload() {
	print_header "Uploading"
	sudo picotool load -f ${project_path}/build/pico2w_minimal.uf2
}

reboot() {
	print_header "Rebooting"
	sudo picotool reboot -f
}

monitor() {
	print_header "Start Monitor"
	sudo arduino-cli monitor -p /dev/ttyACM0 --config 115200;
}

declare -A commands=([b]=build [c]=compile [u]=upload [r]=reboot [m]=monitor)

execute_commands() {
	if declare -f "$1" > /dev/null; then
		"$@"
		exit 0
	fi

	string=$1
	length=${#string}
	for ((i = 0; i < length; i++)); do
		char="${string:i:1}"
		$"${commands[$char]}"

		if [[ $? != 0 ]] && [[ $char != "r" ]]; then
			exit 1
		fi
	done
}


command=$1
shift

# set_flags
rm_build=false
while getopts 'r' flag; do
	case "${flag}" in
		r) rm_build=true ;;
		*) print_usage
			exit 1 ;;
	esac
done

cd ${project_path}

# if [ $command -eq 0 ]; then
# 	bcum
# 	exit 0
if declare -f $command > /dev/null; then
	# "$@"
	$command
	exit 0
else
	execute_commands $command
	exit 0
fi

exit 1
