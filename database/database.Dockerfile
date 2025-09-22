FROM mongo:7.0
LABEL authors="ConnorC432"

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Etc/UTC

RUN apt update && apt upgrade -y

RUN apt update \
    && apt install -y --no-install-recommends \
        nodejs \
        npm \
    && apt clean

COPY schema /docker-entrypoint-initdb.d/schema
COPY init-mongo.js /docker-entrypoint-initdb.d/init-mongo.js

EXPOSE 27017