# Udvikling af h-bro til pakkelevering

I takt med at encoderen ikke længere fungerer hensigtsmæssigt er det muligt at implementere styring af motoren ved brug af en h-bro. Dette er nødvendigt, da fabrikanten af motoren/encoderen og dertilhørende motherboard Maxxon/Escon ikke har gjort det muligt at styre motoren i tilfælde hvor encoderen er i udu.

H-broen vil blive følge et meget klassisk design, da motoren ikke kommer til at kræve mere end ``18V max`` og en strøm på `2A max`. Dette gør, at der blot kan drages inspiration fra nettet til designet.

## Onsdag d. 14-12-22
---

Foreslaget for brug af h-bro blev godtaget og vi arbejder på nuværende med at lave et veroboard design. Komponenterne er skrevet op og afhentet, så vi i morgen er klar til at samle kredsløbet.

## Torsdag d. 15-12-22
---

Design af veroboard er færdiggjort og alle komponenterne er loddet på. Der er blevet foretaget connectivity test og det hele ser ud som det skal. Dette gør at vi i morgen burde være klar til at udvikle noget dertilhørende software og få sat systemet i payload.

## Fredag d. 16-12-22
---

Der er foretaget yderligere test for h-broen. Vi har forbundet motor som load til udgangen og en power supply med maks spænding på 18V, 2A. De udførte test foregik som forventet og resulterede i, at vi havde mulighed for at skrue op/ned for motorens hastighed og ændre dens retning.

## Mandag d. 19-12-22
---

Software for styring af motoren og ændringer af dens hastighed under levering er blevet sat op. Desuden er switch-state medtaget, således at servo stadig kan åbnes ved `'ned'` og motoren kan wind-in ved `'op'`.

## Tirsdag d. 20-12-22
---

Der er blevet foretaget test med motoren. Denne kører som forventet, desuden reagerer servoen også fornuftigt på de angivne signaler. Der er blot for at teste systemet og få det monteret på dronen, så alt er klar til de 'rigtige' endurance test.

### **Fik en idé. Det kan være muligt at blot tælle "FAST" ned hver gang man har eksekveret et loop i main, hvorfra hastigheden falder løbende indtil en bestemt værdi ELLER indtil en bestemt timer tid er nået**
