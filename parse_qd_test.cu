else if (type == "qd")
{
std::string posToken, uToken, vToken, albedoToken, emissionToken;
float px, py, pz, ux, uy, uz, vx, vy, vz;
float r, g, b, roughness, metalness, transmission, refraction_idx;
float er, eg, eb, emission_strength;

if (!(ss >> posToken >> uToken >> vToken >> albedoToken >> roughness >> metalness >> transmission >> refraction_idx >> emissionToken >> emission_strength))
{
std::cerr << "Error de formato en la línea " << lineNumber << " (Quad): argumentos insuficientes. " << line << std::endl;
return;
}

if (!parseVector3Token(posToken, px, py, pz) || !parseVector3Token(uToken, ux, uy, uz) || !parseVector3Token(vToken, vx, vy, vz) || !parseVector3Token(albedoToken, r, g, b) || !parseVector3Token(emissionToken, er, eg, eb))
{
std::cerr << "Error de formato en la línea " << lineNumber << " (Quad): vectores/colores mal formateados (esperado x,y,z). " << line << std::endl;
return;
}

char texturesPaths[5][255];
for (int i = 0; i < 5; ++i)
texturesPaths[i][0] = '\0';

std::string token;
while (ss >> token)
{
if (token.length() > 2 && token[1] == ':')
{
int texIndex = -1;
char prefix = token[0];
if (prefix == 'a')      texIndex = 0; // albedo
else if (prefix == 'r') texIndex = 1; // roughness
else if (prefix == 'm') texIndex = 2; // metalness
else if (prefix == 'o') texIndex = 3; // transmission
else if (prefix == 'e') texIndex = 4; // emission

if (texIndex != -1)
{
std::string path = token.substr(2);
strncpy(texturesPaths[texIndex], path.c_str(), 254);
texturesPaths[texIndex][254] = '\0'; // Asegurar final en null
}
}
}

makeQuad(Point3(px, py, pz), Vector3(ux, uy, uz), Vector3(vx, vy, vz), worldGeometries);

// Añadimos el material 
addMaterial(Color(r, g, b), roughness, metalness, transmission, refraction_idx, Color(er, eg, eb), emission_strength, texturesPaths, materialsList);

std::cout << "Quad creado correctamente en la línea " << lineNumber << std::endl;
}
