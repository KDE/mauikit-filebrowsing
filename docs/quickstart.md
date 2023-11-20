Quick start tutorial {#quickstart}
===================

## Generating the documentation

The tool for generating dox is `src/kapidox_generate`.
Change to an empty directory, then simply point it at the
folder you want to generate dox for (such as a frameworks checkout).

For example, if you have a checkout of KCoreAddons at
~/kde/src/frameworks/kcoreaddons, you could run

    ~/kde/src/frameworks/kapidox/src/kapidox_generate ~/kde/src/frameworks/kcoreaddons

and it would create a documentation in the current directory, which needs to be empty before executing the command.

kapidox recursively walks through folders, so you can also run it on
`~/kde/src/frameworks` or `~/src`. For a lot of libraries, the generation can last
15-30 minutes and use several hundreds of MB, so be prepared!

Pass the --help argument to see options that control the behaviour of the
script.

 
