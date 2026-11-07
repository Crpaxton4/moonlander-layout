# QMK compiles keymap.c only as an include of quantum/keymap_introspection.c, so
# its database has no keymap.c entry. Editors then cannot resolve QMK_KEYBOARD_H
# in keymap.c. The new entry copies the introspection command, with the prelude
# that introspection includes before keymap.c
"quantum/keymap_introspection.c" as $introspection
| [.[] | select(.file == $introspection)] as $entries
| if ($entries | length) != 1 then
    error("expected one \($introspection) entry, found \($entries | length)")
else
    .
end
| map(select(.file != $keymap))
| . + [
    $entries[0]
    | .file = $keymap
    | .arguments |= map(
        if . == $introspection then
            ("-include", "community_modules_introspection.h", $keymap)
        else
            .
        end
    )
]
