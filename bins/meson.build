bins_dir = meson.global_source_root() / get_option('bins')

versions = {
  '1.2/base': '1.2',
  '1.2/sp2': '1.2sp2',
  '1.2/sp2p3': '1.2sp2p3',
  '1.2/sp3': '1.2sp3',
  '1.2/sp4': '1.2sp4',
  '2.0/base': '2.0',
  '2.0/sp1': '2.0sp1',
  '2.0/sp1p2': '2.0sp1p2',
  '2.0/sp1p5': '2.0sp1p5',
  '2.0/sp1p6': '2.0sp1p6',
  '2.0/sp1p7': '2.0sp1p7',
  '2.0/sp2': '2.0sp2',
  '2.0/sp2p2': '2.0sp2p2',
  '2.0/sp2p3': '2.0sp2p3',
  '2.0/sp2p4': '2.0sp2p4',
  'dsi/1.1': 'dsi-1.1',
  'dsi/1.1p1': 'dsi-1.1p1',
  'dsi/1.2': 'dsi-1.2',
  'dsi/1.2p1': 'dsi-1.2p1',
  'dsi/1.2p2': 'dsi-1.2p2',
  'dsi/1.3': 'dsi-1.3',
  'dsi/1.3p1': 'dsi-1.3p1',
  'dsi/1.6sp1': 'dsi-1.6sp1',
  'dsi/1.6sp2': 'dsi-1.6sp2'
}

bins = {}
bins_paths = {}
foreach version, name : versions
  gen = {
    'mwccarm-' + name: 'mwccarm' / version / 'mwccarm.exe',
    'mwasmarm-' + name: 'mwccarm' / version / 'mwasmarm.exe',
    'mwldarm-' + name: 'mwccarm' / version / 'mwldarm.exe'
  }
  foreach name, path: gen
    bins_paths += {name: path}
    bins += {name: files(bins_dir / path)}
  endforeach
endforeach
