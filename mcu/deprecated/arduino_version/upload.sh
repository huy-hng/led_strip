port="/dev/ttyACM0"
board="rp2040:rp2040:rpipico2w"
# compile_cmd="arduino-cli compile -b $board $1"
# upload_cmd="arduino-cli upload -b $board $1 -p $port"
# compile_cmd="arduino-cli compile -b $board"

compile_cmd="arduino-cli compile --quiet -b $board -u -p $port"
upload_cmd="arduino-cli upload -b $board -p $port"

project_path="/home/huy/repositories/led_strip/pico"

sudo chmod o+rw $port

cd ${project_path}

compile() {
	echo
	echo '----------------Compiling----------------'
	echo
	if [[ -z "${LD_LIBRARY_PATH}" ]]; then
		nix-shell ${project_path}/shell.nix --command "$compile_cmd; return"
	else
		$compile_cmd
	fi
}

upload() {
	echo
	echo '----------------Uploading----------------'
	echo
	$upload_cmd
	echo
}

monitor() {
	sudo chmod o+rw $port
	arduino-cli monitor -p /dev/ttyACM0 --config 115200;
}

cum() {
	compile
	if [[ $? == 0 ]]; then
		# upload
		monitor
	fi
}


if [ $# -eq 0 ]; then
	compile
	# if [[ $? == 0 ]]; then
	# 	upload
	# fi
	exit 0
elif declare -f "$1" > /dev/null; then
	"$@"
	exit 0
else
	echo "'$1' is not a known function name" >&2
	exit 1
fi
