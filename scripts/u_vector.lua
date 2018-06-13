Vector = Class:extend()

function Vector:init(x, y, z)
    self.x = x
    self.y = y
    self.z = z
end

function Vector:equal(v)
	if self.x == v.x and self.y == v.y and self.z == v.z then
		return true
	end
	return false
end

function Vector:add(v)
	return Vector(self.x + v.x, self.y + v.y, self.z + v.z)
end

function Vector:subtract(v)
	return Vector(self.x - v.x, self.y - v.y, self.z - v.z)
end

function Vector:distance(v)
	xx = self.x - v.x
	yy = self.y - v.y
	zz = self.z - v.z
	return math.sqrt(xx * xx + yy * yy + zz * zz)
end

function Vector:multiply(s)
	return Vector(self.x*s, self.y*s, self.z*s)
end

function Vector:divide(s)
	return Vector(self.x/s, self.y/s, self.z/s)
end

function Vector:normalize()
	l = math.sqrt(self.x*self.x + self.y*self.y + self.z*self.z)
	if l > 0 then
		return Vector(self.x/l, self.y/l, self.z/l)
	end
	return Vector(self.x,self.y,self.z)
end

function Vector:__call(...)
  local obj = setmetatable({}, self)
  obj:init(...)
  return obj
end

function Vector:__tostring()
	return "Vector "..tostring(self.x).." "..tostring(self.y).." "..tostring(self.z)
end