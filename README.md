# SIC 2025
AVALIAÇÃO DE ASPECTOS DE DISSEMINAÇÃO DE INFORMAÇÃO EM REDES VEICULARES AD HOC.

## Clonar a pasta do projeto
`make copy-sociable`

## Criar imagem docker
`make build-docker-image`

## Abrir container pelo Dev Containers
`Ctrl + Shift + P`  
`Open Folder in Container...`  
`.../ic-gradis-sociable-fork/`

## Abrir container pelo terminal
`docker-compose exec ns3 bash`

## Rodar a simulação (dentro do dev container)
`make configure`  
`make run`  
`make rundb` (modo debug)
