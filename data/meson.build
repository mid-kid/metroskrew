args = []
foreach x : bins_paths_all
  args += [x[0], x[1]]
endforeach

custom_target(
  output: 'sdk',
  command: [
    'sh', '-c',
    '''
      out="$1"; shift
      rm -rf "$out"
      while [ "$#" -ge 2 ]; do
        name="$1"; path="$2"; shift 2
        mkdir -p "$out/${path%/*}"
        printf %s\n "$name" > "$out/$path.txt"
      done
    ''', '--', '@OUTPUT@', args
  ],
  install: true,
  install_dir: get_option('datadir') / 'metroskrew')
