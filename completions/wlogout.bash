_wlogout()
{
    local cur prev
    _get_comp_words_by_ref cur prev

    short=(
        -h
        -l
        -v
        -C
        -b
        -m
        -L
        -R
        -T
        -B
        -r
        -c
        -p
        -P
        -n
    )

    long=(
        --help
        --layout
        --version
        --css
        --buttons-per-row
        --margin
        --margin-left
        --margin-right
        --margin-top
        --margin-bottom
        --column-spacing
        --row-spacing
        --protocol
        --no-span
        --primary-monitor
    )

    case $prev in
        -l|--layout)
            _filedir
            return
            ;;
        -c|--css)
            _filedir
            return
            ;;
    esac

    if [[ $cur == --* ]]; then
        COMPREPLY=($(compgen -W "${long[*]}" -- "$cur"))
    elif [[ $cur == -* ]]; then
        COMPREPLY=($(compgen -W "${short[*]}" -- "$cur"))
        COMPREPLY+=($(compgen -W "${long[*]}" -- "$cur"))
    else
        COMPREPLY=($(compgen -W "${short[*]}" -- "$cur"))
        COMPREPLY+=($(compgen -W "${long[*]}" -- "$cur"))
        COMPREPLY+=($(compgen -c -- "$cur"))
    fi
} &&
complete -F _wlogout wlogout
