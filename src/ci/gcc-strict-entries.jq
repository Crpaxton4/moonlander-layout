# Only the userspace files get entries, because QMK's own files do not compile
# without findings at these warning levels
def in_userspace: . == $userspace or startswith($userspace + "/");

# QMK adds the include directories and the predefined macros of gcc for clang
# tools, and gcc stops on a new definition of __STDC__. -o and the dependency
# flags write files into the QMK build directory
["-o", "-MF", "-MT", "-MQ", "-isystem"] as $with_value
| ["-MMD", "-MP", "-MD"] as $without_value
| map(select(.file | in_userspace))
| if length == 0 then
    error("found no entries in \($userspace)")
else
    .
end
| map(.arguments |= (
    . as $arguments
    | [
        range(length)
        | select(
            ($arguments[.] | IN($with_value[], $without_value[]) | not)
            and ($arguments[.] | startswith("-D__") | not)
            and (. == 0 or ($arguments[. - 1] | IN($with_value[]) | not))
        )
        | $arguments[.]
    ]
    # gcc gives no findings in system headers, so QMK's headers become system
    # headers. Findings in the userspace headers stay
    | map(
        if startswith("-I") and (.[2:] | in_userspace | not) then
            ("-isystem", .[2:])
        else
            .
        end
    )
    # -fanalyzer and -Wnull-dereference give no findings with -fsyntax-only, so
    # gcc compiles to /dev/null
    + ($flags | split(" ")) + ["-Werror", "-o", "/dev/null"]
))
