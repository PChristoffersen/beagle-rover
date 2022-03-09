#!/bin/sh


GENERATED=generated
OUTPUT=src/services
SPEC=../src/robotweb/data/openapi.yaml

EXEC=node_modules/.bin/openapi


cd `dirname $0`/client

rm -rf $OUTPUT/models
$EXEC --input $SPEC --output $GENERATED --exportCore false --exportServices false


cp $GENERATED/index.ts $OUTPUT/model.ts
rsync -va --delete $GENERATED/models/ $OUTPUT/models