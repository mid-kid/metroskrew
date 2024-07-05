args = []
foreach x : bins_paths_all
  args += [x[0] + '.exe', x[1]]
endforeach

custom_target(
  output: 'sdk',
  command: ['sdk.sh', '@OUTPUT@', args],
  install: true,
  install_dir: get_option('datadir') / 'metroskrew')
