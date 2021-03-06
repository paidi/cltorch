luaunit = require('luaunit')
require 'cltorch'

function test_basic()
  luaunit.assertEquals('\n[torch.ClStorage of size 0]\n', tostring(torch.ClStorage()))
  luaunit.assertStrContains(tostring(torch.ClStorage(3)), '\n[torch.ClStorage of size 3]\n')
  luaunit.assertEquals(tostring(torch.ClStorage{4,9,2}), '\n 4\n 9\n 2\n[torch.ClStorage of size 3]\n')
  luaunit.assertEquals(tostring(torch.ClStorage{1.5,2.4,5.3}), '\n 1.5000\n 2.4000\n 5.3000\n[torch.ClStorage of size 3]\n')

  c = torch.ClStorage{4,9,2}
  c:fill(7)
  luaunit.assertEquals(tostring(c), '\n 7\n 7\n 7\n[torch.ClStorage of size 3]\n')

  c = torch.ClStorage{4,9,2}
  c:copy(torch.Storage{1.5,2.4,5.3})
  luaunit.assertEquals(tostring(c), '\n 1.5000\n 2.4000\n 5.3000\n[torch.ClStorage of size 3]\n')

  a = torch.Storage(3)
  c = torch.ClStorage{4,9,2}
  a:copy(c)  
  luaunit.assertEquals(tostring(a), '\n 4\n 9\n 2\n[torch.DoubleStorage of size 3]\n')

  c = torch.ClStorage{4,9,2}
  c[2] = 21
  luaunit.assertEquals(tostring(c), '\n  4\n 21\n  2\n[torch.ClStorage of size 3]\n')

  c = torch.ClStorage{4,9,2}
  d = torch.ClStorage(3)
  d:copy(c)
  luaunit.assertEquals(tostring(d), '\n 4\n 9\n 2\n[torch.ClStorage of size 3]\n')
end

os.exit( luaunit.LuaUnit.run() )

