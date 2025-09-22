FROM node:20-alpine
LABEL authors="ConnorC432"

WORKDIR /app

COPY package*.json ./

RUN npm install --production

COPY . .

EXPOSE 3000

CMD ["npm", "start"]