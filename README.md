# IC Gradis Sociable Fork
Este repositório é um fork do projeto SOCIABLE desenvolvido pelo laboratório LUMO da UFPB, com alterações na função RelayElection.

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
