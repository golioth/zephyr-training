'''my_west_extension.py

Basic example of a west extension.'''

from textwrap import dedent            # just for nicer code indentation

from west.commands import WestCommand  # your extension must subclass this
from west import log                   # use this for user output

import os, time, shutil

class KASM(WestCommand):

    def __init__(self):
        super().__init__(
            'kasm',  # gets stored as self.name
            'Help commands using a Kasm container for Golioth developer training',  # self.help
            # self.description:
            dedent('''
            Golioth developer training includes the option of using a docker
            container with the Zephyr tools that is accessible in the browser
            using the Kasm platform. However, west flash cannot be used because
            the USB port isn\'t available in the container.

            Using `west kasm download` adds a timestamp to the compiled binary
            file and places it into the Kasm download folder. By default, the
            output is placed in ~/Desktop/Downloads as this is a predefined
            location in Kasm. The output directory can be specified using the -o
            flag.'''))

    def do_add_parser(self, parser_adder):
        # This is a bit of boilerplate, which allows you full control over the
        # type of argparse handling you want. The "parser_adder" argument is
        # the return value of an argparse.ArgumentParser.add_subparsers() call.
        parser = parser_adder.add_parser(self.name,
                                         help=self.help,
                                         description=self.description)

        # Add some example options using the standard argparse module API.
        parser.add_argument('command', choices=['download'],
                help='merge binaries and move to kasm download folder')

        parser.add_argument('-d', '--build-dir', default='build', help='build directory to use')
        parser.add_argument('-o', '--output-dir', default='~/Desktop/Downloads', help='output directory to use')

        return parser           # gets stored as self.parser

    def get_cmake_src_dir(self, build_dir):
        with open(os.path.join(build_dir, 'CMakeCache.txt')) as f:
            cmake_lines = f.readlines()
        for line in cmake_lines:
            if line.startswith("APPLICATION_SOURCE_DIR:PATH="):
                app_path = line.split("APPLICATION_SOURCE_DIR:PATH=")[1]
                folder = os.path.basename(app_path).strip()
                return folder
        return "unknown"

    def do_run(self, args, unknown_args):
        # This gets called when the user runs the command, e.g.:
        #
        #   $ west my-command-name -o FOO BAR
        #   --optional is FOO
        #   required is BAR
        if args.command == "download":
            #os.path.join only works if build_dir doesn't have a leading slash
            build_dir = os.path.join(os.getcwd(), os.path.expanduser(args.build_dir))
            output_dir = os.path.join(os.getcwd(), os.path.expanduser(args.output_dir))

            if not os.path.exists(build_dir):
                log.die('cannot find build directory: ', build_dir)
            if not os.path.exists(output_dir):
                log.die('cannot find output directory: ', output_dir)

            app_name = self.get_cmake_src_dir(build_dir)
            format_filename = 'zephyr_{}_%H%M%S.hex'.format(app_name)

            new_filename = time.strftime(format_filename, time.localtime())
            download_file_path=os.path.join(output_dir, new_filename)
            zephyrhex_path=os.path.join(build_dir, 'zephyr/zephyr.hex')

            if not os.path.exists(zephyrhex_path):
                log.die('cannot find zephyr.hex: ', zephyrhex_path)

            try:
                shutil.copyfile(zephyrhex_path, download_file_path)
            except Exception as e:
                log.die("Erro: Unable to copy file for download.", e.message, e.args)

            log.inf("Successfully copied binary to:", download_file_path)
