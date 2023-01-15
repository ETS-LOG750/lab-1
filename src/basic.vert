#version 400 core

// Vertex information from VBO and VAO (vertex attribute)
layout(location = 0) in vec4 vPos; // Position (loc = 0)

// Rappel: Il est obligatoire de spécifier une valeur pour gl_Position
void main()
{
     // Pour le laboratoire 1 seulement :
     // Changement d'un système main gauche vers un système main droite en inversant le Z. 
     // Cela permet que les surfaces ayant un z plus grand se trouve plus proche de la camera
     gl_Position = vec4(vPos.x, vPos.y, -vPos.z, 1);

     // Rappel: Les sommets (ou geometrie) en dehors du cube (-1, 1) ne seront pas visible
}

