function torch.ClTensor.apply(self, func)
   local x = torch.FloatTensor(self:size()):copy(self)
   x:apply(func)
   self:copy(x)
   return self
end

local function Tensor__type(self,type)
   local current = torch.typename(self)
   if not type then return current end
   if type ~= current then
      local new = torch.getmetatable(type).new()
      if self:nElement() > 0 then
         new:resize(self:size()):copy(self)
      end
      return new
   else
      return self
   end
end
local function Tensor__typeAs(self,tensor)
   return self:type(tensor:type())
end
local function Tensor__cl(self)
   return self:type('torch.ClTensor')
end
local function Tensor__double(self)
   return self:type('torch.DoubleTensor')
end
local function Tensor__float(self)
   return self:type('torch.FloatTensor')
end

local function Tensor__byte(self)
   return self:type('torch.ByteTensor')
end

local function Tensor__char(self)
   return self:type('torch.CharTensor')
end

local function Tensor__int(self)
   return self:type('torch.IntTensor')
end

local function Tensor__short(self)
   return self:type('torch.ShortTensor')
end

local function Tensor__long(self)
   return self:type('torch.LongTensor')
end

rawset(torch.getmetatable('torch.DoubleTensor'), 'cl', Tensor__cl)
rawset(torch.getmetatable('torch.FloatTensor'), 'cl', Tensor__cl)
rawset(torch.getmetatable('torch.ByteTensor'), 'cl', Tensor__cl)
rawset(torch.getmetatable('torch.CharTensor'), 'cl', Tensor__cl)
rawset(torch.getmetatable('torch.IntTensor'), 'cl', Tensor__cl)
rawset(torch.getmetatable('torch.ShortTensor'), 'cl', Tensor__cl)
rawset(torch.getmetatable('torch.LongTensor'), 'cl', Tensor__cl)
rawset(torch.getmetatable('torch.ClTensor'), 'cl', Tensor__cl)

rawset(torch.getmetatable('torch.ClTensor'), 'type', Tensor__type)
rawset(torch.getmetatable('torch.ClTensor'), 'typeAs', Tensor__typeAs)
rawset(torch.getmetatable('torch.ClTensor'), 'double', Tensor__double)
rawset(torch.getmetatable('torch.ClTensor'), 'float', Tensor__float)
rawset(torch.getmetatable('torch.ClTensor'), 'byte', Tensor__byte)
rawset(torch.getmetatable('torch.ClTensor'), 'char', Tensor__char)
rawset(torch.getmetatable('torch.ClTensor'), 'int', Tensor__int)
rawset(torch.getmetatable('torch.ClTensor'), 'short', Tensor__short)
rawset(torch.getmetatable('torch.ClTensor'), 'long', Tensor__long)

do
    local metatable = torch.getmetatable('torch.ClTensor')
    for _,func in pairs{'expand', 'expandAs', 'view', 'viewAs', 'repeatTensor',
                        'permute', 'split', 'chunk'} do
        rawset(metatable, func, torch[func])
    end
end
