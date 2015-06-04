--print('done')

--require 'nn'

--a = nn.Linear(2,3)
--print(a)

--a:cl()

--b = torch.Storage(5)
--print(b)

-- c = torch.ClStorage(3)
-- print(c)

-- this function doesnt work
function table_to_str(table)
  results = '{'
  for k,v in pairs(table) do
    results = results .. k .. '=' .. v .. ' '
    -- print(k, ':', v)
  end
  results = results .. '}'
  return results
end

print("running require clnn...")
require 'clnn'
print("... require clnn done")

for k,v in pairs(clnn) do
  print('clnn k,v', k, v)
end

props = clnn.getDeviceProperties(1)
print('props', props)
for k,v in pairs(props) do
  print('props k,v', k, v)
end

print('num devices:', clnn.getDeviceCount())

c = torch.ClStorage()
print('c1', c)

c = torch.ClStorage(3)
print('c2', c)
c[1] = 5
print('c3', c)
c[3] = 7
print('c4', c)
-- print('c' .. c)

c = torch.ClStorage{4,9,2}
print('c5', c)

c:fill(7)
print('c6', c)

a = torch.Storage{1.5, 2.4, 5.3}
print('a', a)

b = torch.Storage(3)
b:copy(a)
print('bbbbb', b)

c:copy(a)
print('c7', c)

c[2] = 21
print('c8', c)
print('aaaaaa', a)
a:copy(c)
print('aaaaaa', a)


