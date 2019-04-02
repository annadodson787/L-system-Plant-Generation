set -e

if [ -z "$1" ]
then
	echo >&2 "No assignment given.";
	exit 1;
fi

assignment=$1
assignment_name=${assignment:3}

cmake --build ./build --config Release --target opengl_viewer
cmake --build ./build --config Release --target $assignment_name

./build/proj/$assignment/$assignment_name -o build/proj/$assignment_name/output

./build/viewer/viewer/opengl_viewer -o build/proj/$assignment_name/output

command -v ffmpeg >/dev/null 2>&1 && ffmpeg -start_number 0 -framerate 50 -i build/proj/$assignment_name/output/_images/%04d.png  -vcodec libx264 -crf 25 -pix_fmt yuv420p -y $assignment_name.mp4
