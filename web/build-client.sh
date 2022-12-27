#!/bin/sh

cd client

rm -rf ../src/robotweb/www
npx browserslist@latest --update-db
npm run build
