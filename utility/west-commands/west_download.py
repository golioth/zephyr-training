'''my_west_extension.py

Basic example of a west extension.'''

from textwrap import dedent            # just for nicer code indentation

from west.commands import WestCommand  # your extension must subclass this
from west import log                   # use this for user output

import os, time, shutil

DEFAULT_DOWNLOAD_DIR = '/zephyr-training/Downloads'

class Download(WestCommand):

    def __init__(self):
        super().__init__(
            'download',  # gets stored as self.name
            'Help commands when using Codespaces for Golioth developer training',  # self.help
            # self.description:
            dedent('''

            Golioth developer training includes the option of using Codespaces
            to provide a build environment in the browser. However, west flash
            cannot be used because the USB port isn\'t available in Codespaces.

            Using `west download` adds a timestamp to the compiled binary file
            and places it into the Downloads folder. By default, the output is
            placed in /workspace/Downloads. The output directory can be
            specified using the -o flag.'''))

    def do_add_parser(self, parser_adder):
        # This is a bit of boilerplate, which allows you full control over the
        # type of argparse handling you want. The "parser_adder" argument is
        # the return value of an argparse.ArgumentParser.add_subparsers() call.
        parser = parser_adder.add_parser(self.name,
                                         help=self.help,
                                         description=self.description)

        # Add some example options using the standard argparse module API.
        parser.add_argument('-d', '--build-dir', default='build', help='build directory to use')
        parser.add_argument('-o', '--output-dir', default=DEFAULT_DOWNLOAD_DIR, help='output directory to use')

        return parser           # gets stored as self.parser

    def get_cmake_values(self, build_dir):
        return_vals = { "cmake_src_dir": None, "board_name": None }
        with open(os.path.join(build_dir, 'CMakeCache.txt')) as f:
            cmake_lines = f.readlines()
        for line in cmake_lines:
            if line.startswith("APPLICATION_SOURCE_DIR:PATH="):
                app_path = line.split("APPLICATION_SOURCE_DIR:PATH=")[1]
                return_vals["cmake_src_dir"] = os.path.basename(app_path).strip()
            elif line.startswith("BOARD:STRING="):
                board_string = line.split("BOARD:STRING=")[1].strip()
                return_vals["board_name"] = board_string
        return return_vals

    def do_run(self, args, unknown_args):
        # This gets called when the user runs the command, e.g.:
        #
        #   $ west my-command-name -o FOO BAR
        #   --optional is FOO
        #   required is BAR

        #os.path.join only works if build_dir doesn't have a leading slash
        build_dir = os.path.join(os.getcwd(), os.path.expanduser(args.build_dir))
        output_dir = os.path.join(os.getcwd(), os.path.expanduser(args.output_dir))

        if not os.path.exists(build_dir):
            log.die('cannot find build directory: ', build_dir)
        if not os.path.exists(output_dir):
            if output_dir == DEFAULT_DOWNLOAD_DIR:
                log.inf("Trying to create Downloads directory:", DEFAULT_DOWNLOAD_DIR)
                try:
                    os.makedirs(DEFAULT_DOWNLOAD_DIR)
                    log.inf("Successfully created Downloads directory.")
                except:
                    log.die('Cannot create directory:', DEFAULT_DOWNLOAD_DIR)
            else:
                log.die('cannot find output directory: ', output_dir)

        cmake_vals = self.get_cmake_values(build_dir)
        board_name = cmake_vals["board_name"]

        if board_name == "nrf7002dk_nrf5340_cpuapp":
            bin_name = 'zephyr.hex'
        elif board_name == "nrf9160dk_nrf9160_ns":
            bin_name = 'merged.hex'
        else:
            log.die("Don't know which binary to move for this board:", board_name)

        app_name = cmake_vals["cmake_src_dir"]
        format_filename = '{}_{}_%H%M%S.hex'.format(board_name.split('_')[0], app_name)

        new_filename = time.strftime(format_filename, time.localtime())
        download_file_path=os.path.join(output_dir, new_filename)
        zephyrhex_path=os.path.join(build_dir, 'zephyr', bin_name)

        if not os.path.exists(zephyrhex_path):
            log.die('cannot find binary: ', zephyrhex_path)

        try:
            shutil.copyfile(zephyrhex_path, download_file_path)
        except Exception as e:
            log.die("Error: Unable to copy file for download.", e.message, e.args)

        log.inf("Successfully copied binary to:", download_file_path)
