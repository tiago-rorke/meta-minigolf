# Meta Mini-Golf

This installation uses the accessibility and familiarity of mini golf, as a medium for playful experimentation with augmented reality, and as an open platform for creating new experiences that incorporate this ritual. Computer vision and projection mapping are using to immerse the player in a virtual, audio-visual experience.

Players can interact however they link, moving the ball on the fairway.  When the ball reaches the hole, the environment will change, taking the view through various experiences, some contributed by different digital artists.

[![](https://farm5.staticflickr.com/4914/45795292124_ee834b3a93_z_d.jpg)](https://www.flickr.com/photos/tiagororke/45795292124/in/album-72157705124534055/)

Primarily developed in [Open Frameworks](https://openframeworks.cc/), tracking data is received via OSC from [Bonsai](https://bonsai-rx.org//), which currently handles the computer vision.  Audio is produced in [Pure Data](https://puredata.info/) and also controlled via OSC.

The ball is 3d printed with a magnetic array below the surface.  A 40mm steel post simulates the hole, and has a corresponding ring of magnets at its base.

[![](https://farm5.staticflickr.com/4884/45795298264_f5cf642907_z_d.jpg)](https://www.flickr.com/photos/tiagororke/45795298264/in/album-72157705124534055/)


## Thank-yous!

This project was developed at the [Expand](http://cienciaviva.pt/projectos/expand.asp) art/science/technology residency at the Espaço do Tempo in Montemor-o-novo, Portugal, December 2018.  Many thanks to everybody involved who made this wonderful residency possible!

Special thanks to:
[Pedro Ângelo](https://github.com/pangelo/) for his expertise in Open Frameworks, [João Frazão](http://neuro.fchampalimaud.org/en/person/244/) & [Gonçalo Lopes](http://neuro.fchampalimaud.org/en/person/18/) for their expertise in Bonsai, [Mick Mengucci](http://misturapura.net/) & Francisco for their expertise in Pure Data, Tiago Frois at the [Oficinas do Convento](http://www.oficinasdoconvento.com/) for his machining expertise, and many others who helped out during the residency :).