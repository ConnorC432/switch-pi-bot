const fs = require('fs');
db = db.getSiblingDB('appConfig');

function mergeDocuments(existingDoc, newDoc) {
  for (const key in newDoc) {
    if (key === 'settings' && Array.isArray(newDoc.settings) && Array.isArray(existingDoc.settings)) {
      const mergedSettings = newDoc.settings.map(newSetting => {
        const existingSetting = existingDoc.settings.find(s => s.argName === newSetting.argName);
        return existingSetting ? { ...newSetting, value: existingSetting.value } : newSetting;
      });
      existingDoc.settings = mergedSettings;
    } else if (typeof newDoc[key] === 'object' && newDoc[key] !== null && typeof existingDoc[key] === 'object' && existingDoc[key] !== null) {
      mergeDocuments(existingDoc[key], newDoc[key]);
    } else if (!(key in existingDoc)) {
      existingDoc[key] = newDoc[key];
    }
  }
  return existingDoc;
}
function upsertCollection(filePath, collectionName, uniqueKey) {
  if (!fs.existsSync(filePath)) {
    print(`File not found: ${filePath}, skipping ${collectionName}`);
    return;
  }

  const data = JSON.parse(fs.readFileSync(filePath));

  if (Array.isArray(data)) {
    if (data.length === 0) {
      // Create empty collection if it doesn't exist
      if (!db.getCollectionNames().includes(collectionName)) {
        db.createCollection(collectionName);
        print(`Created empty collection: ${collectionName}`);
      }
      return;
    }

    data.forEach(doc => {
      const existing = db[collectionName].findOne({ [uniqueKey]: doc[uniqueKey] });
      if (existing) {
        const merged = mergeDocuments(existing, doc);
        db[collectionName].updateOne({ [uniqueKey]: doc[uniqueKey] }, { $set: merged });
      } else {
        db[collectionName].insertOne(doc);
      }
    });
  } else if (typeof data === 'object' && Object.keys(data).length > 0) {
    const existing = db[collectionName].findOne({ [uniqueKey]: data[uniqueKey] });
    if (existing) {
      const merged = mergeDocuments(existing, data);
      db[collectionName].updateOne({ [uniqueKey]: data[uniqueKey] }, { $set: merged });
    } else {
      db[collectionName].insertOne(data);
    }
  } else {
    if (!db.getCollectionNames().includes(collectionName)) {
      db.createCollection(collectionName);
      print(`Created empty collection: ${collectionName}`);
    }
  }
}

upsertCollection('/docker-entrypoint-initdb.d/schema/globalSettings.json', 'globalSettings', 'setting');
upsertCollection('/docker-entrypoint-initdb.d/schema/localSettings.json', 'localSettings', 'setting');
upsertCollection('/docker-entrypoint-initdb.d/schema/programs.json', 'programs', 'programName');

print("MongoDB init complete.");